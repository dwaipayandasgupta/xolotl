// Includes
#include "interface.h"
#include <iostream>
#include <Options.h>
#include <IMaterialFactory.h>
#include <TemperatureHandlerFactory.h>
#include <VizHandlerRegistryFactory.h>
#include <xolotlPerf.h>
#include <SolverHandlerFactory.h>
#include <IReactionHandlerFactory.h>
#include <ctime>
#include <MPIUtils.h>

void XolotlInterface::printSomething() {
	std::cout << "I'm in Xolotl !!!" << std::endl;
	return;
}

std::shared_ptr<xolotlSolver::PetscSolver> XolotlInterface::initializeXolotl(
		int argc, char **argv, MPI_Comm comm, bool isStandalone) {
	// Local Declarations
	std::shared_ptr<xolotlSolver::PetscSolver> solver;

	// Initialize the MPI communicator to use
	xolotlCore::MPIUtils::initialize(comm);
	auto xolotlComm = xolotlCore::MPIUtils::getMPIComm();

	// Get the MPI rank
	int rank;
	MPI_Comm_rank(xolotlComm, &rank);

	if (rank == 0) {
		// Print the start message
		std::cout << "Starting Xolotl Plasma-Surface Interactions Simulator"
				<< std::endl;
		// TODO! Print copyright message
		// Print date and time
		std::time_t currentTime = std::time(NULL);
		std::cout << std::asctime(std::localtime(&currentTime));
	}

	try {
		// Skip the executable name before parsing
		argc -= 1; // one for the executable name
		argv += 1; // one for the executable name
		Options opts;
		opts.readParams(argv);
		if (!opts.shouldRun()) {
			std::cerr << "Unable to read the options.  Aborting" << std::endl;
		}

		// Set up our performance data infrastructure.
		xolotlPerf::initialize(opts.getPerfHandlerType());
		auto handlerRegistry = xolotlPerf::getHandlerRegistry();

		// Create the material factory
		auto materialFactory =
				xolotlFactory::IMaterialFactory::createMaterialFactory(
						opts.getMaterial(), opts.getDimensionNumber());
		// Initialize it with the options
		materialFactory->initializeMaterial(opts);

		// Initialize the temperature
		if (!xolotlFactory::initializeTempHandler(opts)) {
			std::cerr << "Unable to initialize requested temperature.  Aborting"
					<< std::endl;
		}
		// Get the temperature handler
		auto tempHandler = xolotlFactory::getTemperatureHandler();

		// Initialize the visualization
		if (!xolotlFactory::initializeVizHandler(
				opts.useVizStandardHandlers())) {
			std::cerr
					<< "Unable to initialize requested visualization infrastructure. "
					<< "Aborting" << std::endl;
		}

		// Create the network handler factory
		auto networkFactory =
				xolotlFactory::IReactionHandlerFactory::createNetworkFactory(
						opts.getMaterial());
		// Build a reaction network
		networkFactory->initializeReactionNetwork(opts, handlerRegistry);
		auto& network = networkFactory->getNetworkHandler();

		// Initialize and get the solver handler
		if (!xolotlFactory::initializeDimension(opts, network)) {
			std::cerr << "Unable to initialize dimension from inputs. "
					<< "Aborting" << std::endl;
		}
		auto& solvHandler = xolotlFactory::getSolverHandler();
		// Initialize the solver handler
		solvHandler.initializeHandlers(materialFactory, tempHandler, opts);

		// Setup the solver
		solver = std::shared_ptr<xolotlSolver::PetscSolver>(
				new xolotlSolver::PetscSolver(solvHandler, handlerRegistry));
		// Initialize the solver
		solver->setCommandLineOptions(opts.getPetscArgv());
		solver->initialize(isStandalone);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (const std::string& error) {
		std::cerr << error << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (...) {
		std::cerr << "Unrecognized exception seen." << std::endl;
		std::cerr << "Aborting." << std::endl;
	}

	return solver;
}

void XolotlInterface::solveXolotl(
		std::shared_ptr<xolotlSolver::PetscSolver> solver) {
	try {
		// Launch the PetscSolver
		solver->solve();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (const std::string& error) {
		std::cerr << error << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (...) {
		std::cerr << "Unrecognized exception seen." << std::endl;
		std::cerr << "Aborting." << std::endl;
	}

	return;
}

std::vector<double> XolotlInterface::getCopyRetention(
		std::shared_ptr<xolotlSolver::PetscSolver> solver) {
	std::vector<double> retention;
	try {
		// Get the solver handler
		auto& solverHandler = solver->getSolverHandler();
		// Get the latest retention value from it
		retention = solverHandler.getCopyRetention();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (const std::string& error) {
		std::cerr << error << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (...) {
		std::cerr << "Unrecognized exception seen." << std::endl;
		std::cerr << "Aborting." << std::endl;
	}

	return retention;
}

std::vector<double>* XolotlInterface::getPointerRetention(
		std::shared_ptr<xolotlSolver::PetscSolver> solver) {
	auto retention = new std::vector<double>();
	try {
		// Get the solver handler
		auto& solverHandler = solver->getSolverHandler();
		// Get the latest retention value from it
		retention = solverHandler.getPointerRetention();
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (const std::string& error) {
		std::cerr << error << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (...) {
		std::cerr << "Unrecognized exception seen." << std::endl;
		std::cerr << "Aborting." << std::endl;
	}

	return retention;
}

void XolotlInterface::printRetention(
		std::shared_ptr<xolotlSolver::PetscSolver> solver) {
	try {

		// Get the MPI rank
		auto xolotlComm = xolotlCore::MPIUtils::getMPIComm();
		int rank;
		MPI_Comm_rank(xolotlComm, &rank);

		if (rank == 0) {
			// Get the solver handler
			auto& solverHandler = solver->getSolverHandler();
			// Get the latest retention value from it
			auto retention = solverHandler.getCopyRetention();
			std::cout << "The retention was: " << std::endl;
			for (int i = 0; i < retention.size(); i++) {
				std::cout << retention[i] << " ";
			}
			std::cout << std::endl;
		}
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (const std::string& error) {
		std::cerr << error << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (...) {
		std::cerr << "Unrecognized exception seen." << std::endl;
		std::cerr << "Aborting." << std::endl;
	}
	return;
}

void XolotlInterface::finalizeXolotl(
		std::shared_ptr<xolotlSolver::PetscSolver> solver, bool isStandalone) {
	try {
		// Call solver finalize
		solver->finalize(isStandalone);
	} catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (const std::string& error) {
		std::cerr << error << std::endl;
		std::cerr << "Aborting." << std::endl;
	} catch (...) {
		std::cerr << "Unrecognized exception seen." << std::endl;
		std::cerr << "Aborting." << std::endl;
	}

	return;
}
