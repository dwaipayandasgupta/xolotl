#ifndef XCORE_XFILE_H
#define XCORE_XFILE_H

#include <string>
#include <vector>
#include <tuple>
#include "xolotlCore/io/HDF5File.h"
#include "xolotlCore/io/HDF5Exception.h"


namespace xolotlCore {

class XFile : public HDF5File {
public:

    // A group with info about a specific time step.
    class ConcentrationGroup;
    class TimestepGroup : public HDF5File::Group {
    private:

        // Prefix to use when constructing group names.
        static std::string groupNamePrefix;

        /**
         * Construct the group name for the given time step.
         *
         * @param concGroup The parent concentration group.
         * @param timeStep The time step the group will represent.
         * @return A string to use for the name of the time step group for
         *          the given time step.
         */
        static std::string makeGroupName(const ConcentrationGroup& concGroup,
                                            int timeStep);

    public:
        TimestepGroup(void) = delete;
        TimestepGroup(const TimestepGroup& other) = delete;

        /**
         * Create and populate a Timestep group within the given 
         * concentration group.
         *
         * @param timeStep The number of the time step
         * @param time The physical time at this time step
         * @param previousTime The physical time at the previous time step
         * @param deltaTime The physical length of the time step
         */
        TimestepGroup(const ConcentrationGroup& concGroup,
                            int timeStep,
                            double time,
                            double previousTime,
                            double deltaTime);

        /**
         * Open a TimestepGroup within the given concentration group.
         *
         * @param timeStep The time step of the desired group.
         */
        TimestepGroup(const ConcentrationGroup& concGroup, int timeStep);


        /**
         * Save the surface positions to our timestep group.
         *
         * @param iSurface The index of the surface position
         * @param nInter The quantity of interstitial at each surface position
         * @param previousFlux The previous I flux at each surface position
         */
        void writeSurface1D(int iSurface, double nInter, double previousFlux);

        /**
         * Save the surface positions to our timestep group.
         *
         * @param iSurface The indices of the surface position
         * @param nInter The quantity of interstitial at each surface position
         * @param previousFlux The previous I flux at each surface position
         */
        void writeSurface2D(std::vector<int> iSurface,
                std::vector<double> nInter, std::vector<double> previousFlux);

        /**
         * Save the surface positions to our timestep group.
         *
         * @param iSurface The indices of the surface position
         * @param nInter The quantity of interstitial at each surface position
         * @param previousFlux The previous I flux at each surface position
         */
        void writeSurface3D(std::vector<std::vector<int> > iSurface,
                std::vector<std::vector<double> > nInter,
                std::vector<std::vector<double> > previousFlux);

        /**
         * Save the bottom informations to our timestep group.
         *
         * @param nHe The quantity of helium at the bottom
         * @param previousHeFlux The previous He flux
         * @param nD The quantity of deuterium at the bottom
         * @param previousDFlux The previous D flux
         * @param nT The quantity of tritium at the bottom
         * @param previousTFlux The previous T flux
         */
        void writeBottom1D(double nHe, double previousHeFlux, double nD,
                double previousDFlux, double nT, double previousTFlux);

        /**
         * Save the bottom informations to our timestep group.
         *
         * @param nHe The quantity of helium at the bottom
         * @param previousHeFlux The previous He flux
         * @param nD The quantity of deuterium at the bottom
         * @param previousDFlux The previous D flux
         * @param nT The quantity of tritium at the bottom
         * @param previousTFlux The previous T flux
         */
        void writeBottom2D(std::vector<double> nHe,
                std::vector<double> previousHeFlux,
                std::vector<double> nD, std::vector<double> previousDFlux,
                std::vector<double> nT, std::vector<double> previousTFlux);


        /**
         * Add a concentration dataset at a specific grid point.
         *
         * @param size The size of the dataset to create
         * @param concArray The array of concentration at a grid point
         * @param i The index of the position on the grid on the x direction
         * @param j The index of the position on the grid on the y direction
         * @param k The index of the position on the grid on the z direction
         */
        void writeConcentrationDataset(int size,
                                        double concArray[][2],
                                        int i, int j = -1, int k = -1);

        /**
         * Read the times from our timestep group.
         *
         * @return pair(time, deltaTime) containing the physical time to 
         *          be changed and the time step length to be changed.
         */
        std::pair<double, double> readTimes(void) const;


        /**
         * Read the previous time from our concentration group.
         *
         * @return The physical time at the previous timestep
         */
        double readPreviousTime(void) const;

        /**
         * Read the surface position from our concentration group in
         * the case of a 1D grid (one surface position).
         *
         * @return The index of the surface position
         */
        int readSurface1D(void) const;

        /**
         * Read the surface position from our concentration group in
         * the case of a 2D grid (a vector of surface positions).
         *
         * @return The vector of indices of the surface position
         */
        std::vector<int> readSurface2D(void) const;

        /**
         * Read the surface position from our concentration group in
         * the case of a 3D grid (a vector of vector of surface positions).
         *
         * @return The vector of vector of indices of the surface position
         */
        std::vector<std::vector<int> > readSurface3D(void) const;

        /**
         * Read some data from our concentration
         * group in the case of a 1D grid (one float).
         *
         * @param dataName The name of the data we want
         * @return The value of the data
         */
        double readData1D(const std::string& dataName) const;

        /**
         * Read some data from our concentration group in
         * the case of a 2D grid (a vector).
         *
         * @param dataName The name of the data we want
         * @return The vector of the data
         */
        std::vector<double> readData2D(const std::string& dataName) const;

        /**
         * Read some data from our concentration group file in
         * the case of a 3D grid (a vector of vector).
         *
         * @param dataName The name of the data we want
         * @return The vector of vector of data
         */
        std::vector<std::vector<double> > readData3D(
                const std::string& dataName) const;

        /**
         * Read our (i,j,k)-th grid point concentrations.
         *
         * @param i The index of the grid point on the x axis
         * @param j The index of the grid point on the y axis
         * @param k The index of the grid point on the z axis
         * @return The vector of concentrations
         */
        std::vector<std::vector<double> > readGridPoint(
                int i, int j = -1, int k = -1) const;

    };


    // Our concentrations group.
    class ConcentrationGroup : public HDF5File::Group {
    private:
        // Path of the concentrations group within the file.
        static const fs::path path;

    public:
        // Create or open the concentrationsGroup.
        ConcentrationGroup(void) = delete;
        ConcentrationGroup(const ConcentrationGroup& other) = delete;
        ConcentrationGroup(const XFile& file, bool create = false);

        /**
         * Add a concentration timestep group for the given time step.
         *
         * @param timeStep The number of the time step
         * @param time The physical time at this time step
         * @param previousTime The physical time at the previous time step
         * @param deltaTime The physical length of the time step
         */
        std::unique_ptr<TimestepGroup> addTimestepGroup(int timeStep,
                                                    double time,
                                                    double previousTime,
                                                    double deltaTime) const;

        /**
         * Obtain the last timestep known to our group.
         *
         * @return Time step of last TimestepGroup written to our group.
         */
        int getLastTimeStep(void) const;


        /**
         * Determine if we have any TimestepGroups.
         *
         * @return True iff any TimestepGroups have been written.
         */
        bool hasTimesteps(void) const { return getLastTimeStep() >= 0; }

        /**
         * Access the TimestepGroup associated with the given time step.
         *
         * @param ts Time step of the desired TimestepGroup.
         * @return TimestepGroup associated with the given time step.  Empty
         *          pointer if the given time step is not known to us.
         */
        std::unique_ptr<TimestepGroup> getTimestepGroup(int timeStep) const;


        /**
         * Access the TimestepGroup associated with the last known time step.
         *
         * @return TimestepGroup associated with the last known time step.
         *          Empty pointer if we do not yet have any time steps.
         */
        std::unique_ptr<TimestepGroup> getLastTimestepGroup(void) const;
    };


    // Our header group.
    class HeaderGroup : public HDF5File::Group {
    private:
        // Path of the header group within the file.
        static const fs::path path;

        /**
         * Initialize the list of cluster compositions.
         *
         * @param headerGroup Group representing our file's header.
         * @param compVec The vector of composition
         */
        void initNetworkComp(const std::vector<std::vector<int>>& compVec);

    public:
        /**
         * Create the header group.
         * Default and copy constructors explicitly disallowed.
         */
        HeaderGroup(void) = delete;
        HeaderGroup(const HeaderGroup& other) = delete;

        /**
         * Create and initialize the header group with the number of 
         * points and step size in each direction.
         *
         * @param file The file in which the header should be added.
         * @param grid The grid points in the x direction (depth)
         * @param ny The number of grid points in the y direction
         * @param hy The step size in the y direction
         * @param nz The number of grid points in the z direction
         * @param hz The step size in the z direction
         */
        HeaderGroup(const XFile& file,
                const std::vector<double>& grid,
                int ny, double hy,
                int nz, double hz,
                const std::vector<std::vector<int> >& compVec);

        /**
         * Open an existing header group.
         */
        HeaderGroup(const XFile& file);

        /**
         * Read our file header.
         *
         * @param nx The number of grid points in the x direction (depth)
         * @param hx The step size in the x direction
         * @param ny The number of grid points in the y direction
         * @param hy The step size in the y direction
         * @param nz The number of grid points in the z direction
         * @param hz The step size in the z direction
         */
        void read(int &nx, double &hx, int &ny,
                double &hy, int &nz, double &hz) const;
    };


    // A group describing a network within our HDF5 file.
    class NetworkGroup : public HDF5File::Group {
    public:
        // Path to the network group within our HDF5 file.
        static const fs::path path;

        NetworkGroup(void) = delete;
        NetworkGroup(const NetworkGroup& other) = delete;

        /**
         * Open an existing network group.
         *
         * @param file The file whose network group to open.
         */
        NetworkGroup(const XFile& file);


        /**
         * Read the network from our group.
         * 
         * @return The vector of vector which contain the network dataset.
         */
        std::vector<std::vector<double> > readNetwork(void) const;

        
        /**
         * Copy ourself to the given file.
         * A NetworkGroup must not already exist in the file.
         *
         * @param target The file to copy ourself to.
         */
        void copyTo(const XFile& target) const;
    };

private:

    /**
     * Pass through only Create* access modes.
     *
     * @param mode The access mode to check.
     * @return The given access mode if it is a Create* mode.  Otherwise, 
     *          throws an exception.
     */
    static AccessMode EnsureCreateAccessMode(AccessMode mode);

    /**
     * Pass through only Open* access modes.
     *
     * @param mode The access mode to check.
     * @return The given access mode if it is a Open* mode.  Otherwise, 
     *          throws an exception.
     */
    static AccessMode EnsureOpenAccessMode(AccessMode mode);


public:
    /**
     * Create and initialize a checkpoint file.
     *
     * @param path Path of file to create.
     * @param grid The grid points in the x direction (depth)
     * @param compVec The composition vector.
     * @param networkFilePath Path to file from which the network will 
     *          be copied.  No network will be copied if networkFilePath 
     *          is empty.
     * @param ny The number of grid points in the y direction
     * @param hy The step size in the y direction
     * @param nz The number of grid points in the z direction
     * @param hz The step size in the z direction
     * @param mode Access mode for file.  Only HDF5File Create* modes 
     *              are supported.
     */
    XFile(fs::path path,
            const std::vector<double>& grid,
            const std::vector<std::vector<int> >& compVec,
            fs::path networkFilePath,
            int ny = 0,
            double hy = 0.0,
            int nz = 0,
            double hz = 0.0,
            AccessMode mode = AccessMode::CreateOrTruncateIfExists);


    /**
     * Open an existing checkpoint or network file.
     *
     * @param path Path of file to open.
     * @param mode Access mode for file.  Only HDFFile Open* modes 
     *              are supported.
     */
    XFile(fs::path path, AccessMode mode = AccessMode::OpenReadOnly);


    /**
     * Access one of our top-level Groups within our file.
     *
     * @return The group object if we can open the group, else an empty pointer.
     */
    template<typename T>
    std::unique_ptr<T> getGroup(void) const {
        
        std::unique_ptr<T> group;

        try {
            // Open the group within our file.
            group.reset(new T(*this));
        }
        catch(HDF5Exception& e) {
            // We were not able to open the desired group.
            // Ensure that the return pointer is empty.
            assert(not group);
        }
        return std::move(group);
    }
};

} /* namespace xolotlCore */

#endif // XCORE_XFILE_H

