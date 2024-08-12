#ifndef AMOEBA_OPENMM_COMMONKERNELS_H_
#define AMOEBA_OPENMM_COMMONKERNELS_H_

/* -------------------------------------------------------------------------- *
 *                              OpenMMAmoeba                                  *
 * -------------------------------------------------------------------------- *
 * This is part of the OpenMM molecular simulation toolkit originating from   *
 * Simbios, the NIH National Center for Physics-Based Simulation of           *
 * Biological Structures at Stanford, funded under the NIH Roadmap for        *
 * Medical Research, grant U54 GM072970. See https://simtk.org.               *
 *                                                                            *
 * Portions copyright (c) 2008-2021 Stanford University and the Authors.      *
 * Authors: Mark Friedrichs, Peter Eastman                                    *
 * Contributors:                                                              *
 *                                                                            *
 * This program is free software: you can redistribute it and/or modify       *
 * it under the terms of the GNU Lesser General Public License as published   *
 * by the Free Software Foundation, either version 3 of the License, or       *
 * (at your option) any later version.                                        *
 *                                                                            *
 * This program is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
 * GNU Lesser General Public License for more details.                        *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
 * -------------------------------------------------------------------------- */

#include "openmm/amoebaKernels.h"
#include "openmm/kernels.h"
#include "openmm/System.h"
#include "openmm/common/ComputeContext.h"
#include "openmm/common/ComputeArray.h"
#include "openmm/common/NonbondedUtilities.h"

namespace OpenMM {

class CommonCalcAmoebaGeneralizedKirkwoodForceKernel;

/**
 * This kernel is invoked by AmoebaTorsionTorsionForce to calculate the forces acting on the system and the energy of the system.
 */
class CommonCalcAmoebaTorsionTorsionForceKernel : public CalcAmoebaTorsionTorsionForceKernel {
public:
    CommonCalcAmoebaTorsionTorsionForceKernel(const std::string& name, const Platform& platform, ComputeContext& cc, const System& system);
    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the AmoebaTorsionTorsionForce this kernel will be used for
     */
    void initialize(const System& system, const AmoebaTorsionTorsionForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
private:
    class ForceInfo;
    int numTorsionTorsions;
    int numTorsionTorsionGrids;
    ComputeContext& cc;
    const System& system;
    ComputeArray gridValues;
    ComputeArray gridParams;
    ComputeArray torsionParams;
};

/**
 * This kernel is invoked by AmoebaMultipoleForce to calculate the forces acting on the system and the energy of the system.
 */
class CommonCalcAmoebaMultipoleForceKernel : public CalcAmoebaMultipoleForceKernel {
public:
    CommonCalcAmoebaMultipoleForceKernel(const std::string& name, const Platform& platform, ComputeContext& cc, const System& system);
    ~CommonCalcAmoebaMultipoleForceKernel();
    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the AmoebaMultipoleForce this kernel will be used for
     */
    void initialize(const System& system, const AmoebaMultipoleForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
     /**
     * Get the LabFrame dipole moments of all particles.
     * 
     * @param context    the Context for which to get the induced dipoles
     * @param dipoles    the induced dipole moment of particle i is stored into the i'th element
     */
    void getLabFramePermanentDipoles(ContextImpl& context, std::vector<Vec3>& dipoles);
    /**
     * Get the induced dipole moments of all particles.
     * 
     * @param context    the Context for which to get the induced dipoles
     * @param dipoles    the induced dipole moment of particle i is stored into the i'th element
     */
    void getInducedDipoles(ContextImpl& context, std::vector<Vec3>& dipoles);
    /**
     * Get the total dipole moments of all particles.
     * 
     * @param context    the Context for which to get the induced dipoles
     * @param dipoles    the induced dipole moment of particle i is stored into the i'th element
     */
    void getTotalDipoles(ContextImpl& context, std::vector<Vec3>& dipoles);
    /**
     * Execute the kernel to calculate the electrostatic potential
     *
     * @param context        the context in which to execute this kernel
     * @param inputGrid      input grid coordinates
     * @param outputElectrostaticPotential output potential 
     */
    void getElectrostaticPotential(ContextImpl& context, const std::vector< Vec3 >& inputGrid,
                                   std::vector< double >& outputElectrostaticPotential);

   /** 
     * Get the system multipole moments
     *
     * @param context      context
     * @param outputMultipoleMoments (charge,
     *                                dipole_x, dipole_y, dipole_z,
     *                                quadrupole_xx, quadrupole_xy, quadrupole_xz,
     *                                quadrupole_yx, quadrupole_yy, quadrupole_yz,
     *                                quadrupole_zx, quadrupole_zy, quadrupole_zz)
     */
    void getSystemMultipoleMoments(ContextImpl& context, std::vector<double>& outputMultipoleMoments);
    /**
     * Copy changed parameters over to a context.
     *
     * @param context    the context to copy parameters to
     * @param force      the AmoebaMultipoleForce to copy the parameters from
     */
    void copyParametersToContext(ContextImpl& context, const AmoebaMultipoleForce& force);
    /**
     * Get the parameters being used for PME.
     * 
     * @param alpha   the separation parameter
     * @param nx      the number of grid points along the X axis
     * @param ny      the number of grid points along the Y axis
     * @param nz      the number of grid points along the Z axis
     */
    void getPMEParameters(double& alpha, int& nx, int& ny, int& nz) const;
    /**
     * Compute the FFT.
     */
    virtual void computeFFT(bool forward) = 0;
    /**
     * Get whether charge spreading should be done in fixed point.
     */
    virtual bool useFixedPointChargeSpreading() const = 0;
protected:
    class ForceInfo;
    void initializeScaleFactors();
    void computeInducedField();
    bool iterateDipolesByDIIS(int iteration);
    void computeExtrapolatedDipoles();
    void ensureMultipolesValid(ContextImpl& context);
    template <class T, class T4, class M4> void computeSystemMultipoleMoments(ContextImpl& context, std::vector<double>& outputMultipoleMoments);
    int numMultipoles, maxInducedIterations, maxExtrapolationOrder;
    int fixedFieldThreads, inducedFieldThreads, electrostaticsThreads;
    int gridSizeX, gridSizeY, gridSizeZ;
    double pmeAlpha, inducedEpsilon;
    bool usePME, hasQuadrupoles, hasInitializedScaleFactors, multipolesAreValid, hasCreatedEvent;
    AmoebaMultipoleForce::PolarizationType polarizationType;
    ComputeContext& cc;
    const System& system;
    std::vector<mm_int4> covalentFlagValues;
    std::vector<mm_int2> polarizationFlagValues;
    ComputeArray multipoleParticles;
    ComputeArray localDipoles;
    ComputeArray localQuadrupoles;
    ComputeArray labDipoles;
    ComputeArray labQuadrupoles;
    ComputeArray sphericalDipoles;
    ComputeArray sphericalQuadrupoles;
    ComputeArray fracDipoles;
    ComputeArray fracQuadrupoles;
    ComputeArray field;
    ComputeArray fieldPolar;
    ComputeArray inducedField;
    ComputeArray inducedFieldPolar;
    ComputeArray torque;
    ComputeArray dampingAndThole;
    ComputeArray inducedDipole;
    ComputeArray inducedDipolePolar;
    ComputeArray inducedDipoleErrors;
    ComputeArray prevDipoles;
    ComputeArray prevDipolesPolar;
    ComputeArray prevDipolesGk;
    ComputeArray prevDipolesGkPolar;
    ComputeArray prevErrors;
    ComputeArray diisMatrix;
    ComputeArray diisCoefficients;
    ComputeArray extrapolatedDipole;
    ComputeArray extrapolatedDipolePolar;
    ComputeArray extrapolatedDipoleGk;
    ComputeArray extrapolatedDipoleGkPolar;
    ComputeArray inducedDipoleFieldGradient;
    ComputeArray inducedDipoleFieldGradientPolar;
    ComputeArray inducedDipoleFieldGradientGk;
    ComputeArray inducedDipoleFieldGradientGkPolar;
    ComputeArray extrapolatedDipoleFieldGradient;
    ComputeArray extrapolatedDipoleFieldGradientPolar;
    ComputeArray extrapolatedDipoleFieldGradientGk;
    ComputeArray extrapolatedDipoleFieldGradientGkPolar;
    ComputeArray polarizability;
    ComputeArray covalentFlags;
    ComputeArray polarizationGroupFlags;
    ComputeArray pmeGrid1;
    ComputeArray pmeGrid2;
    ComputeArray pmeGridLong;
    ComputeArray pmeBsplineModuliX;
    ComputeArray pmeBsplineModuliY;
    ComputeArray pmeBsplineModuliZ;
    ComputeArray pmePhi;
    ComputeArray pmePhid;
    ComputeArray pmePhip;
    ComputeArray pmePhidp;
    ComputeArray pmeCphi;
    ComputeArray lastPositions;
    ComputeKernel computeMomentsKernel, recordInducedDipolesKernel, mapTorqueKernel, computePotentialKernel, electrostaticsKernel;
    ComputeKernel computeFixedFieldKernel, computeInducedFieldKernel, updateInducedFieldKernel;
    ComputeKernel recordDIISDipolesKernel, buildMatrixKernel, solveMatrixKernel;
    ComputeKernel initExtrapolatedKernel, iterateExtrapolatedKernel, computeExtrapolatedKernel, addExtrapolatedGradientKernel;
    ComputeKernel pmeSpreadFixedMultipolesKernel, pmeSpreadInducedDipolesKernel, pmeFinishSpreadChargeKernel, pmeConvolutionKernel;
    ComputeKernel pmeFixedPotentialKernel, pmeInducedPotentialKernel, pmeFixedForceKernel, pmeInducedForceKernel, pmeRecordInducedFieldDipolesKernel;
    ComputeKernel pmeTransformMultipolesKernel, pmeTransformPotentialKernel;
    ComputeEvent syncEvent;
    CommonCalcAmoebaGeneralizedKirkwoodForceKernel* gkKernel;
    static const int PmeOrder = 5;
    static const int MaxPrevDIISDipoles = 20;
};

/**
 * This kernel is invoked by AmoebaMultipoleForce to calculate the forces acting on the system and the energy of the system.
 */
class CommonCalcAmoebaGeneralizedKirkwoodForceKernel : public CalcAmoebaGeneralizedKirkwoodForceKernel {
public:
    CommonCalcAmoebaGeneralizedKirkwoodForceKernel(const std::string& name, const Platform& platform, ComputeContext& cc, const System& system);
    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the AmoebaMultipoleForce this kernel will be used for
     */
    void initialize(const System& system, const AmoebaGeneralizedKirkwoodForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    /**
     * Perform the computation of Born radii.
     */
    void computeBornRadii(ComputeArray& torque, ComputeArray& labFrameDipoles, ComputeArray& labFrameQuadrupoles, ComputeArray& inducedDipole, ComputeArray& inducedDipolePolar, ComputeArray& dampingAndThole, ComputeArray& covalentFlags, ComputeArray& polarizationGroupFlags);
    /**
     * Perform the final parts of the force/energy computation.
     */
    void finishComputation();
    ComputeArray& getBornRadii() {
        return bornRadii;
    }
    ComputeArray& getField() {
        return field;
    }
    ComputeArray& getInducedField() {
        return inducedField;
    }
    ComputeArray& getInducedFieldPolar() {
        return inducedFieldPolar;
    }
    ComputeArray& getInducedDipoles() {
        return inducedDipoleS;
    }
    ComputeArray& getInducedDipolesPolar() {
        return inducedDipolePolarS;
    }
    /**
     * Copy changed parameters over to a context.
     *
     * @param context    the context to copy parameters to
     * @param force      the AmoebaGeneralizedKirkwoodForce to copy the parameters from
     */
    void copyParametersToContext(ContextImpl& context, const AmoebaGeneralizedKirkwoodForce& force);

        const static int NUM_NECK_POINTS = 45;

        const float benchmarkedRadii[NUM_NECK_POINTS] = {
                0.80, 0.85, 0.90, 0.95, 1.00, 1.05, 1.10, 1.15, 1.20, 1.25,
                1.30, 1.35, 1.40, 1.45, 1.50, 1.55, 1.60, 1.65, 1.70, 1.75,
                1.80, 1.85, 1.90, 1.95, 2.00, 2.05, 2.10, 2.15, 2.20, 2.25,
                2.30, 2.35, 2.40, 2.45, 2.50, 2.55, 2.60, 2.65, 2.70, 2.75,
                2.80, 2.85, 2.90, 2.95, 3.00};

        const double MINIMUM_RADIUS = 0.80;
        const double MAXIMUM_RADIUS = 3.00;
        const double SPACING = 0.05;

        const float Aij[NUM_NECK_POINTS][NUM_NECK_POINTS] = {
                {0.0000577616, 0.0000584661, 0.0000363925, 0.0000395472, 0.0000443202, 0.0000485507, 0.0000430862,
                        0.0000485067, 0.0000244504, 0.0000278293, 0.0000329908, 0.0000292135, 0.0000343621, 0.0000393724,
                        0.0000352501, 0.0000303823, 0.0000360595, 0.0000418690, 0.0000365804, 0.0000248824, 0.0000375656,
                        0.0000428918, 0.0000377450, 0.0000447160, 0.0000395375, 0.0000345934, 0.0000536114, 0.0000470958,
                        0.0000542111, 0.0000360263, 0.0000553398, 0.0000483197, 0.0000555466, 0.0000374290, 0.0000327412,
                        0.0000386635, 0.0000578345, 0.0000513194, 0.0000581077, 0.0000394022, 0.0000599546, 0.0000392112,
                        0.0000597613, 0.0000403186, 0.0000615149},
                {0.0000446374, 0.0000460309, 0.0000475714, 0.0000225974, 0.0000318052, 0.0000275537, 0.0000296576,
                        0.0000339157, 0.0000295595, 0.0000344661, 0.0000395587, 0.0000197502, 0.0000300923, 0.0000272524,
                        0.0000187561, 0.0000272680, 0.0000247659, 0.0000214933, 0.0000252850, 0.0000223386, 0.0000261633,
                        0.0000398363, 0.0000363492, 0.0000412599, 0.0000363327, 0.0000422768, 0.0000367723, 0.0000425282,
                        0.0000374607, 0.0000325975, 0.0000383563, 0.0000253707, 0.0000301193, 0.0000200866, 0.0000299808,
                        0.0000263514, 0.0000304240, 0.0000349497, 0.0000408533, 0.0000268003, 0.0000407538, 0.0000357651,
                        0.0000418473, 0.0000278226, 0.0000419066},
                {0.0000360721, 0.0000365794, 0.0000372415, 0.0000303637, 0.0000319835, 0.0000267215, 0.0000171087,
                        0.0000252702, 0.0000221094, 0.0000187747, 0.0000212341, 0.0000251617, 0.0000215761, 0.0000191079,
                        0.0000291389, 0.0000199177, 0.0000288958, 0.0000260343, 0.0000179669, 0.0000210581, 0.0000178820,
                        0.0000215615, 0.0000243911, 0.0000220007, 0.0000249689, 0.0000295075, 0.0000260062, 0.0000297050,
                        0.0000203093, 0.0000305802, 0.0000202272, 0.0000308984, 0.0000207930, 0.0000316764, 0.0000277135,
                        0.0000316983, 0.0000219362, 0.0000326865, 0.0000286724, 0.0000332929, 0.0000220858, 0.0000332871,
                        0.0000294921, 0.0000348374, 0.0000230989},
                {0.0000398429, 0.0000223074, 0.0000298279, 0.0000315221, 0.0000187855, 0.0000264807, 0.0000169459,
                        0.0000180824, 0.0000162496, 0.0000233148, 0.0000157394, 0.0000174931, 0.0000152866, 0.0000230620,
                        0.0000205539, 0.0000234872, 0.0000213273, 0.0000181123, 0.0000210938, 0.0000186307, 0.0000216437,
                        0.0000146084, 0.0000222850, 0.0000149429, 0.0000170964, 0.0000153536, 0.0000177037, 0.0000208958,
                        0.0000237412, 0.0000164214, 0.0000239561, 0.0000166849, 0.0000188702, 0.0000167221, 0.0000188448,
                        0.0000172644, 0.0000194706, 0.0000174585, 0.0000259642, 0.0000177409, 0.0000199219, 0.0000235990,
                        0.0000202504, 0.0000234495, 0.0000156052},
                {0.0000435754, 0.0000245527, 0.0000318302, 0.0000188461, 0.0000192238, 0.0000208396, 0.0000169209,
                        0.0000183816, 0.0000204363, 0.0000131412, 0.0000193141, 0.0000097655, 0.0000110841, 0.0000129627,
                        0.0000112003, 0.0000130467, 0.0000113669, 0.0000168195, 0.0000117549, 0.0000134876, 0.0000118881,
                        0.0000180528, 0.0000159126, 0.0000181928, 0.0000125940, 0.0000183351, 0.0000125678, 0.0000145035,
                        0.0000131847, 0.0000144906, 0.0000129840, 0.0000150912, 0.0000176485, 0.0000153063, 0.0000178114,
                        0.0000157934, 0.0000180763, 0.0000157725, 0.0000186835, 0.0000121847, 0.0000185069, 0.0000161900,
                        0.0000191223, 0.0000126643, 0.0000192108},
                {0.0000505580, 0.0000264633, 0.0000205166, 0.0000195164, 0.0000204987, 0.0000160556, 0.0000127648,
                        0.0000137952, 0.0000155458, 0.0000176400, 0.0000146241, 0.0000124027, 0.0000103247, 0.0000120745,
                        0.0000105222, 0.0000124882, 0.0000140152, 0.0000095449, 0.0000141168, 0.0000074383, 0.0000142994,
                        0.0000098671, 0.0000112045, 0.0000132832, 0.0000113876, 0.0000135139, 0.0000116918, 0.0000137152,
                        0.0000118068, 0.0000140495, 0.0000093075, 0.0000144645, 0.0000126001, 0.0000142255, 0.0000128571,
                        0.0000148412, 0.0000128819, 0.0000152166, 0.0000099735, 0.0000151882, 0.0000133944, 0.0000154829,
                        0.0000135105, 0.0000154771, 0.0000106154},
                {0.0000449657, 0.0000230612, 0.0000221541, 0.0000166532, 0.0000171555, 0.0000227174, 0.0000182899,
                        0.0000193198, 0.0000118768, 0.0000130396, 0.0000113361, 0.0000093799, 0.0000108573, 0.0000120843,
                        0.0000106342, 0.0000117938, 0.0000080569, 0.0000091361, 0.0000107718, 0.0000123647, 0.0000106453,
                        0.0000123678, 0.0000109954, 0.0000126151, 0.0000145692, 0.0000128038, 0.0000146248, 0.0000099091,
                        0.0000114200, 0.0000059787, 0.0000117991, 0.0000136370, 0.0000119830, 0.0000080880, 0.0000120147,
                        0.0000108970, 0.0000161522, 0.0000083711, 0.0000094980, 0.0000112647, 0.0000096319, 0.0000113677,
                        0.0000127823, 0.0000114549, 0.0000100471},
                {0.0000280510, 0.0000266023, 0.0000144227, 0.0000188499, 0.0000184712, 0.0000146008, 0.0000195724,
                        0.0000161268, 0.0000073492, 0.0000137704, 0.0000111866, 0.0000072795, 0.0000081037, 0.0000092319,
                        0.0000078144, 0.0000089354, 0.0000102556, 0.0000090005, 0.0000059425, 0.0000090194, 0.0000080186,
                        0.0000069127, 0.0000082105, 0.0000093797, 0.0000082401, 0.0000093675, 0.0000082671, 0.0000071727,
                        0.0000064889, 0.0000097376, 0.0000087729, 0.0000098019, 0.0000088064, 0.0000059057, 0.0000089896,
                        0.0000077635, 0.0000070336, 0.0000047870, 0.0000094273, 0.0000080466, 0.0000093429, 0.0000083051,
                        0.0000096725, 0.0000109657, 0.0000097148},
                {0.0000325317, 0.0000295175, 0.0000213868, 0.0000208878, 0.0000153087, 0.0000116695, 0.0000091353,
                        0.0000128753, 0.0000102693, 0.0000107086, 0.0000088500, 0.0000098955, 0.0000081413, 0.0000052255,
                        0.0000078083, 0.0000067997, 0.0000098737, 0.0000066979, 0.0000098507, 0.0000051252, 0.0000074307,
                        0.0000067922, 0.0000057491, 0.0000068223, 0.0000058920, 0.0000070420, 0.0000060742, 0.0000069635,
                        0.0000062687, 0.0000054473, 0.0000061911, 0.0000073394, 0.0000037836, 0.0000074661, 0.0000084983,
                        0.0000073838, 0.0000087027, 0.0000076952, 0.0000088636, 0.0000102208, 0.0000090108, 0.0000104452,
                        0.0000090944, 0.0000104765, 0.0000070232},
                {0.0000289111, 0.0000198943, 0.0000241993, 0.0000238536, 0.0000182705, 0.0000129878, 0.0000076255,
                        0.0000102719, 0.0000112418, 0.0000067659, 0.0000071756, 0.0000100155, 0.0000065300, 0.0000071811,
                        0.0000060818, 0.0000066768, 0.0000059369, 0.0000065993, 0.0000076971, 0.0000063984, 0.0000057069,
                        0.0000086185, 0.0000076581, 0.0000051018, 0.0000057661, 0.0000051292, 0.0000058121, 0.0000050955,
                        0.0000079236, 0.0000069082, 0.0000060582, 0.0000041682, 0.0000080245, 0.0000042302, 0.0000082046,
                        0.0000056029, 0.0000048280, 0.0000057624, 0.0000065014, 0.0000058246, 0.0000049969, 0.0000058491,
                        0.0000066001, 0.0000059999, 0.0000088507},
                {0.0000265006, 0.0000302279, 0.0000215102, 0.0000203741, 0.0000195197, 0.0000114271, 0.0000110442,
                        0.0000113577, 0.0000067378, 0.0000072687, 0.0000074058, 0.0000047733, 0.0000067184, 0.0000076291,
                        0.0000062951, 0.0000051957, 0.0000044871, 0.0000050768, 0.0000044211, 0.0000049880, 0.0000043518,
                        0.0000048796, 0.0000057445, 0.0000049576, 0.0000057224, 0.0000037798, 0.0000043883, 0.0000066161,
                        0.0000044279, 0.0000050580, 0.0000045758, 0.0000051723, 0.0000046936, 0.0000052703, 0.0000047182,
                        0.0000041352, 0.0000063088, 0.0000041672, 0.0000063473, 0.0000055519, 0.0000049210, 0.0000033576,
                        0.0000049766, 0.0000033829, 0.0000051220},
                {0.0000393068, 0.0000205177, 0.0000254533, 0.0000229604, 0.0000129838, 0.0000126427, 0.0000096583,
                        0.0000123670, 0.0000100764, 0.0000100625, 0.0000047514, 0.0000049466, 0.0000041283, 0.0000059449,
                        0.0000065208, 0.0000072136, 0.0000045347, 0.0000039272, 0.0000033654, 0.0000038633, 0.0000032913,
                        0.0000038211, 0.0000041976, 0.0000037860, 0.0000042838, 0.0000037697, 0.0000043641, 0.0000038556,
                        0.0000033089, 0.0000039242, 0.0000043859, 0.0000051678, 0.0000034265, 0.0000040012, 0.0000035224,
                        0.0000053110, 0.0000046531, 0.0000053869, 0.0000036188, 0.0000054308, 0.0000048054, 0.0000055242,
                        0.0000048512, 0.0000056159, 0.0000037853},
                {0.0000340677, 0.0000307596, 0.0000223668, 0.0000152946, 0.0000146712, 0.0000106255, 0.0000106032,
                        0.0000081832, 0.0000086244, 0.0000049411, 0.0000067545, 0.0000040421, 0.0000044178, 0.0000036241,
                        0.0000052208, 0.0000032905, 0.0000048780, 0.0000053199, 0.0000046459, 0.0000039440, 0.0000026124,
                        0.0000050097, 0.0000026014, 0.0000029505, 0.0000032710, 0.0000037980, 0.0000043410, 0.0000037740,
                        0.0000033662, 0.0000050175, 0.0000033958, 0.0000051015, 0.0000034086, 0.0000030335, 0.0000034235,
                        0.0000039828, 0.0000045344, 0.0000031386, 0.0000046458, 0.0000041624, 0.0000036086, 0.0000041947,
                        0.0000028209, 0.0000033460, 0.0000028613},
                {0.0000305973, 0.0000272968, 0.0000142276, 0.0000179127, 0.0000097246, 0.0000127507, 0.0000070473,
                        0.0000090593, 0.0000092161, 0.0000074076, 0.0000074685, 0.0000058617, 0.0000047016, 0.0000051588,
                        0.0000056952, 0.0000035730, 0.0000038387, 0.0000042402, 0.0000035579, 0.0000018248, 0.0000046133,
                        0.0000039370, 0.0000045159, 0.0000029244, 0.0000019710, 0.0000022822, 0.0000025439, 0.0000028944,
                        0.0000025756, 0.0000029250, 0.0000034260, 0.0000050361, 0.0000034605, 0.0000038688, 0.0000026949,
                        0.0000014049, 0.0000027321, 0.0000023567, 0.0000027913, 0.0000031218, 0.0000027763, 0.0000031969,
                        0.0000037163, 0.0000032316, 0.0000037723},
                {0.0000363524, 0.0000185238, 0.0000223944, 0.0000156641, 0.0000146928, 0.0000107770, 0.0000105276,
                        0.0000102180, 0.0000079337, 0.0000078580, 0.0000062276, 0.0000066384, 0.0000039738, 0.0000055727,
                        0.0000060414, 0.0000037461, 0.0000023444, 0.0000033734, 0.0000028739, 0.0000032262, 0.0000026725,
                        0.0000030603, 0.0000025932, 0.0000030096, 0.0000019791, 0.0000029386, 0.0000033334, 0.0000029324,
                        0.0000025033, 0.0000017645, 0.0000014959, 0.0000022899, 0.0000025847, 0.0000030273, 0.0000026349,
                        0.0000039712, 0.0000026279, 0.0000040671, 0.0000020776, 0.0000023832, 0.0000020939, 0.0000024085,
                        0.0000021149, 0.0000041807, 0.0000016785},
                {0.0000531068, 0.0000206882, 0.0000259899, 0.0000178808, 0.0000168038, 0.0000128055, 0.0000119227,
                        0.0000069103, 0.0000090425, 0.0000068286, 0.0000069793, 0.0000031585, 0.0000042989, 0.0000027028,
                        0.0000036738, 0.0000031193, 0.0000043434, 0.0000035227, 0.0000030391, 0.0000024934, 0.0000027944,
                        0.0000024067, 0.0000027907, 0.0000023644, 0.0000026945, 0.0000022659, 0.0000020013, 0.0000029627,
                        0.0000026172, 0.0000017442, 0.0000025980, 0.0000017461, 0.0000020037, 0.0000017818, 0.0000026313,
                        0.0000023219, 0.0000026522, 0.0000023901, 0.0000026985, 0.0000014157, 0.0000027079, 0.0000018522,
                        0.0000016312, 0.0000019179, 0.0000016703},
                {0.0000471466, 0.0000321678, 0.0000221166, 0.0000210526, 0.0000114910, 0.0000108287, 0.0000080777,
                        0.0000104203, 0.0000101134, 0.0000058443, 0.0000045067, 0.0000045975, 0.0000027454, 0.0000037782,
                        0.0000039918, 0.0000043093, 0.0000046405, 0.0000029403, 0.0000023896, 0.0000020792, 0.0000029414,
                        0.0000014845, 0.0000016390, 0.0000018155, 0.0000020600, 0.0000018173, 0.0000020305, 0.0000018045,
                        0.0000020275, 0.0000023056, 0.0000026380, 0.0000022906, 0.0000015633, 0.0000022861, 0.0000015702,
                        0.0000022925, 0.0000015926, 0.0000017741, 0.0000015991, 0.0000017969, 0.0000021445, 0.0000018491,
                        0.0000021663, 0.0000018937, 0.0000021829},
                {0.0000415033, 0.0000375035, 0.0000257317, 0.0000181483, 0.0000170401, 0.0000165719, 0.0000091912,
                        0.0000089667, 0.0000066295, 0.0000051660, 0.0000051868, 0.0000051552, 0.0000041581, 0.0000041949,
                        0.0000044142, 0.0000035882, 0.0000029449, 0.0000031798, 0.0000020273, 0.0000021690, 0.0000024377,
                        0.0000026775, 0.0000029680, 0.0000014909, 0.0000021480, 0.0000018690, 0.0000027312, 0.0000014150,
                        0.0000020266, 0.0000013942, 0.0000015678, 0.0000014176, 0.0000015601, 0.0000018280, 0.0000020424,
                        0.0000023951, 0.0000015782, 0.0000024117, 0.0000016197, 0.0000014155, 0.0000012558, 0.0000018686,
                        0.0000016359, 0.0000018949, 0.0000010017},
                {0.0000468296, 0.0000330068, 0.0000237247, 0.0000165336, 0.0000150520, 0.0000109676, 0.0000103584,
                        0.0000059806, 0.0000075019, 0.0000075639, 0.0000075577, 0.0000044171, 0.0000035406, 0.0000027896,
                        0.0000038483, 0.0000030165, 0.0000024502, 0.0000020229, 0.0000037725, 0.0000023248, 0.0000025725,
                        0.0000016526, 0.0000018416, 0.0000020307, 0.0000023292, 0.0000019462, 0.0000022344, 0.0000014452,
                        0.0000016743, 0.0000024751, 0.0000021503, 0.0000010914, 0.0000016308, 0.0000018570, 0.0000021221,
                        0.0000010982, 0.0000016425, 0.0000011133, 0.0000021459, 0.0000018960, 0.0000021557, 0.0000011360,
                        0.0000012755, 0.0000011503, 0.0000012923},
                {0.0000326803, 0.0000295857, 0.0000154475, 0.0000195504, 0.0000133752, 0.0000130826, 0.0000093108,
                        0.0000090846, 0.0000051384, 0.0000065258, 0.0000049655, 0.0000050318, 0.0000039301, 0.0000031064,
                        0.0000018937, 0.0000033983, 0.0000027192, 0.0000021941, 0.0000023262, 0.0000011684, 0.0000015864,
                        0.0000017653, 0.0000019394, 0.0000021477, 0.0000018003, 0.0000015654, 0.0000017271, 0.0000019541,
                        0.0000012911, 0.0000008739, 0.0000016831, 0.0000011063, 0.0000016687, 0.0000014377, 0.0000016576,
                        0.0000010945, 0.0000012820, 0.0000010911, 0.0000007782, 0.0000014342, 0.0000013165, 0.0000018913,
                        0.0000013170, 0.0000008899, 0.0000013269},
                {0.0000381583, 0.0000264565, 0.0000136612, 0.0000212466, 0.0000153348, 0.0000145316, 0.0000138739,
                        0.0000060648, 0.0000076347, 0.0000074893, 0.0000043282, 0.0000057183, 0.0000033840, 0.0000045556,
                        0.0000035441, 0.0000028461, 0.0000022853, 0.0000031916, 0.0000026005, 0.0000021084, 0.0000013651,
                        0.0000024956, 0.0000016110, 0.0000017750, 0.0000014880, 0.0000016813, 0.0000010862, 0.0000016184,
                        0.0000013616, 0.0000015542, 0.0000017206, 0.0000015449, 0.0000013086, 0.0000015137, 0.0000012941,
                        0.0000011585, 0.0000012793, 0.0000008977, 0.0000009980, 0.0000011586, 0.0000010254, 0.0000011655,
                        0.0000010218, 0.0000011671, 0.0000010320},
                {0.0000435206, 0.0000394602, 0.0000280329, 0.0000143528, 0.0000103966, 0.0000099195, 0.0000123591,
                        0.0000070629, 0.0000087274, 0.0000066090, 0.0000048718, 0.0000038172, 0.0000050202, 0.0000029810,
                        0.0000040378, 0.0000024193, 0.0000024874, 0.0000015771, 0.0000016403, 0.0000017939, 0.0000019398,
                        0.0000012193, 0.0000017244, 0.0000008475, 0.0000009524, 0.0000008041, 0.0000011807, 0.0000009651,
                        0.0000011501, 0.0000012202, 0.0000010892, 0.0000011977, 0.0000010599, 0.0000015547, 0.0000010348,
                        0.0000011765, 0.0000010241, 0.0000009188, 0.0000007943, 0.0000009237, 0.0000010314, 0.0000009055,
                        0.0000010352, 0.0000009196, 0.0000013384},
                {0.0000379248, 0.0000353934, 0.0000241508, 0.0000168891, 0.0000210529, 0.0000086678, 0.0000083510,
                        0.0000106765, 0.0000058348, 0.0000057662, 0.0000074621, 0.0000043406, 0.0000033715, 0.0000044195,
                        0.0000020552, 0.0000027516, 0.0000016638, 0.0000022213, 0.0000023581, 0.0000019527, 0.0000016003,
                        0.0000017368, 0.0000008516, 0.0000009200, 0.0000016828, 0.0000008623, 0.0000009286, 0.0000006197,
                        0.0000011498, 0.0000007848, 0.0000006628, 0.0000009788, 0.0000010822, 0.0000012385, 0.0000008152,
                        0.0000009435, 0.0000010686, 0.0000012014, 0.0000006276, 0.0000007030, 0.0000010605, 0.0000012119,
                        0.0000006389, 0.0000007195, 0.0000006456},
                {0.0000457913, 0.0000308903, 0.0000282516, 0.0000197423, 0.0000139036, 0.0000076554, 0.0000101164,
                        0.0000092286, 0.0000068324, 0.0000052054, 0.0000050091, 0.0000049163, 0.0000039143, 0.0000029462,
                        0.0000039499, 0.0000031099, 0.0000024405, 0.0000014864, 0.0000012048, 0.0000021759, 0.0000013931,
                        0.0000014556, 0.0000009208, 0.0000017108, 0.0000011004, 0.0000011923, 0.0000007721, 0.0000006708,
                        0.0000005731, 0.0000008092, 0.0000009072, 0.0000007914, 0.0000008763, 0.0000007716, 0.0000006536,
                        0.0000007653, 0.0000008561, 0.0000009956, 0.0000008472, 0.0000007539, 0.0000008319, 0.0000005765,
                        0.0000008451, 0.0000009713, 0.0000008480},
                {0.0000514751, 0.0000274557, 0.0000241481, 0.0000174373, 0.0000161298, 0.0000116871, 0.0000084217,
                        0.0000081004, 0.0000058484, 0.0000056938, 0.0000057532, 0.0000043019, 0.0000032347, 0.0000025748,
                        0.0000034234, 0.0000026896, 0.0000016029, 0.0000016452, 0.0000023736, 0.0000013932, 0.0000011273,
                        0.0000009500, 0.0000013052, 0.0000010750, 0.0000012044, 0.0000010077, 0.0000011046, 0.0000007011,
                        0.0000010268, 0.0000008714, 0.0000012822, 0.0000010702, 0.0000007283, 0.0000010366, 0.0000005479,
                        0.0000007812, 0.0000005332, 0.0000007670, 0.0000006899, 0.0000007700, 0.0000006826, 0.0000007637,
                        0.0000008807, 0.0000004626, 0.0000006769},
                {0.0000347279, 0.0000326038, 0.0000222389, 0.0000203032, 0.0000186389, 0.0000135396, 0.0000074878,
                        0.0000093982, 0.0000069447, 0.0000051305, 0.0000038953, 0.0000049720, 0.0000038543, 0.0000038303,
                        0.0000017387, 0.0000013908, 0.0000018492, 0.0000018676, 0.0000011554, 0.0000016034, 0.0000009913,
                        0.0000010369, 0.0000011030, 0.0000015670, 0.0000013066, 0.0000011031, 0.0000009195, 0.0000007778,
                        0.0000010981, 0.0000009562, 0.0000010346, 0.0000011776, 0.0000007583, 0.0000005111, 0.0000005726,
                        0.0000011004, 0.0000009317, 0.0000004887, 0.0000005500, 0.0000008051, 0.0000005530, 0.0000004756,
                        0.0000005513, 0.0000006031, 0.0000005515},
                {0.0000404627, 0.0000279374, 0.0000254737, 0.0000173416, 0.0000166276, 0.0000090743, 0.0000084501,
                        0.0000108659, 0.0000080225, 0.0000045088, 0.0000044169, 0.0000057245, 0.0000033186, 0.0000043660,
                        0.0000034171, 0.0000015607, 0.0000020522, 0.0000012726, 0.0000016999, 0.0000013417, 0.0000018688,
                        0.0000015245, 0.0000009452, 0.0000007794, 0.0000008405, 0.0000011825, 0.0000007598, 0.0000008210,
                        0.0000007108, 0.0000007647, 0.0000006612, 0.0000007339, 0.0000008145, 0.0000007040, 0.0000006095,
                        0.0000008740, 0.0000003513, 0.0000006717, 0.0000005660, 0.0000004997, 0.0000009522, 0.0000005060,
                        0.0000005619, 0.0000005063, 0.0000004323},
                {0.0000472063, 0.0000323179, 0.0000294752, 0.0000207416, 0.0000113045, 0.0000136112, 0.0000129076,
                        0.0000071735, 0.0000091135, 0.0000052733, 0.0000039191, 0.0000038507, 0.0000022739, 0.0000029313,
                        0.0000017476, 0.0000029817, 0.0000017682, 0.0000014044, 0.0000018903, 0.0000011418, 0.0000015945,
                        0.0000009870, 0.0000010526, 0.0000011160, 0.0000011997, 0.0000013110, 0.0000010830, 0.0000006980,
                        0.0000009891, 0.0000006386, 0.0000005495, 0.0000006040, 0.0000006782, 0.0000004347, 0.0000004971,
                        0.0000004232, 0.0000004838, 0.0000006935, 0.0000003617, 0.0000004015, 0.0000003611, 0.0000003155,
                        0.0000005902, 0.0000003968, 0.0000004545},
                {0.0000534708, 0.0000373744, 0.0000196796, 0.0000137544, 0.0000129458, 0.0000119235, 0.0000088305,
                        0.0000111632, 0.0000062367, 0.0000059625, 0.0000034417, 0.0000044561, 0.0000033301, 0.0000025232,
                        0.0000019586, 0.0000027071, 0.0000015804, 0.0000012263, 0.0000016555, 0.0000017219, 0.0000010458,
                        0.0000008561, 0.0000006975, 0.0000009600, 0.0000013346, 0.0000008501, 0.0000004210, 0.0000005776,
                        0.0000008267, 0.0000009169, 0.0000004551, 0.0000005061, 0.0000007113, 0.0000006128, 0.0000006790,
                        0.0000004531, 0.0000005155, 0.0000005740, 0.0000006449, 0.0000002634, 0.0000004837, 0.0000005547,
                        0.0000004790, 0.0000004165, 0.0000004781},
                {0.0000471509, 0.0000331027, 0.0000226179, 0.0000161311, 0.0000144893, 0.0000107687, 0.0000104009,
                        0.0000128476, 0.0000071831, 0.0000053734, 0.0000030440, 0.0000051224, 0.0000039650, 0.0000022452,
                        0.0000017674, 0.0000017430, 0.0000017813, 0.0000014173, 0.0000018887, 0.0000008757, 0.0000012001,
                        0.0000016279, 0.0000012946, 0.0000004971, 0.0000008642, 0.0000005591, 0.0000006041, 0.0000006461,
                        0.0000005270, 0.0000007557, 0.0000004882, 0.0000007062, 0.0000005940, 0.0000008734, 0.0000005615,
                        0.0000004848, 0.0000004202, 0.0000004705, 0.0000004021, 0.0000004565, 0.0000003960, 0.0000004488,
                        0.0000004998, 0.0000005792, 0.0000003821},
                {0.0000408712, 0.0000290825, 0.0000201809, 0.0000184693, 0.0000127543, 0.0000122856, 0.0000089579,
                        0.0000087349, 0.0000048255, 0.0000062188, 0.0000059289, 0.0000044984, 0.0000019907, 0.0000033642,
                        0.0000034197, 0.0000026227, 0.0000012039, 0.0000015836, 0.0000009945, 0.0000013051, 0.0000017849,
                        0.0000010646, 0.0000005240, 0.0000007062, 0.0000005767, 0.0000006087, 0.0000008726, 0.0000005467,
                        0.0000007758, 0.0000006348, 0.0000007039, 0.0000003514, 0.0000006567, 0.0000002671, 0.0000006217,
                        0.0000006907, 0.0000005847, 0.0000001470, 0.0000004367, 0.0000003746, 0.0000003283, 0.0000003639,
                        0.0000004146, 0.0000003630, 0.0000004069},
                {0.0000365867, 0.0000334092, 0.0000233935, 0.0000211266, 0.0000148841, 0.0000145388, 0.0000102065,
                        0.0000099718, 0.0000095120, 0.0000041586, 0.0000052071, 0.0000030423, 0.0000030232, 0.0000022968,
                        0.0000023000, 0.0000030274, 0.0000022943, 0.0000010722, 0.0000011058, 0.0000014789, 0.0000009014,
                        0.0000009248, 0.0000009830, 0.0000010354, 0.0000014149, 0.0000011942, 0.0000005606, 0.0000006027,
                        0.0000005065, 0.0000004217, 0.0000005883, 0.0000005132, 0.0000003302, 0.0000004589, 0.0000006648,
                        0.0000003397, 0.0000004773, 0.0000003190, 0.0000004559, 0.0000003103, 0.0000005849, 0.0000003016,
                        0.0000005635, 0.0000003806, 0.0000002617},
                {0.0000424740, 0.0000299616, 0.0000274655, 0.0000188374, 0.0000175698, 0.0000126091, 0.0000089948,
                        0.0000068101, 0.0000083814, 0.0000048968, 0.0000060247, 0.0000059532, 0.0000020417, 0.0000044737,
                        0.0000025919, 0.0000020128, 0.0000020369, 0.0000012489, 0.0000016173, 0.0000012975, 0.0000013224,
                        0.0000010382, 0.0000008390, 0.0000008790, 0.0000009452, 0.0000009921, 0.0000006307, 0.0000006689,
                        0.0000003338, 0.0000007868, 0.0000005076, 0.0000004219, 0.0000003654, 0.0000008632, 0.0000004278,
                        0.0000003612, 0.0000002445, 0.0000005864, 0.0000002315, 0.0000004357, 0.0000002247, 0.0000004156,
                        0.0000002193, 0.0000003178, 0.0000002730},
                {0.0000503599, 0.0000451538, 0.0000316354, 0.0000217532, 0.0000202497, 0.0000144337, 0.0000104958,
                        0.0000100705, 0.0000074034, 0.0000054909, 0.0000041189, 0.0000040598, 0.0000030289, 0.0000030085,
                        0.0000029895, 0.0000023332, 0.0000018003, 0.0000013931, 0.0000018829, 0.0000014630, 0.0000006967,
                        0.0000009205, 0.0000012416, 0.0000007812, 0.0000010583, 0.0000006536, 0.0000009038, 0.0000004477,
                        0.0000010522, 0.0000006597, 0.0000005567, 0.0000004616, 0.0000006575, 0.0000003344, 0.0000002766,
                        0.0000003963, 0.0000005688, 0.0000001793, 0.0000004153, 0.0000004620, 0.0000003992, 0.0000004404,
                        0.0000002976, 0.0000004364, 0.0000003740},
                {0.0000430595, 0.0000397799, 0.0000271616, 0.0000143472, 0.0000176142, 0.0000168235, 0.0000093500,
                        0.0000067329, 0.0000066446, 0.0000082213, 0.0000061390, 0.0000046372, 0.0000034811, 0.0000034735,
                        0.0000026730, 0.0000026634, 0.0000015581, 0.0000021099, 0.0000012381, 0.0000016679, 0.0000010186,
                        0.0000010483, 0.0000008241, 0.0000011290, 0.0000005475, 0.0000005724, 0.0000004661, 0.0000006409,
                        0.0000004075, 0.0000005660, 0.0000004676, 0.0000006678, 0.0000005536, 0.0000004588, 0.0000003086,
                        0.0000002600, 0.0000002221, 0.0000004089, 0.0000005910, 0.0000002319, 0.0000003286, 0.0000002849,
                        0.0000003188, 0.0000001718, 0.0000003117},
                {0.0000503956, 0.0000262755, 0.0000184690, 0.0000169051, 0.0000157560, 0.0000113206, 0.0000108187,
                        0.0000079141, 0.0000075536, 0.0000055140, 0.0000054540, 0.0000040614, 0.0000039965, 0.0000023509,
                        0.0000030468, 0.0000018116, 0.0000023451, 0.0000013890, 0.0000014094, 0.0000011387, 0.0000011426,
                        0.0000007091, 0.0000009704, 0.0000007647, 0.0000008029, 0.0000011008, 0.0000003155, 0.0000004312,
                        0.0000004596, 0.0000004817, 0.0000005269, 0.0000005676, 0.0000003706, 0.0000004006, 0.0000004315,
                        0.0000004775, 0.0000003098, 0.0000003407, 0.0000004886, 0.0000004152, 0.0000001682, 0.0000003093,
                        0.0000002068, 0.0000002292, 0.0000002008},
                {0.0000334173, 0.0000309726, 0.0000218877, 0.0000196442, 0.0000138546, 0.0000130794, 0.0000094927,
                        0.0000068714, 0.0000086744, 0.0000063898, 0.0000037817, 0.0000048166, 0.0000046345, 0.0000026610,
                        0.0000015855, 0.0000020759, 0.0000027141, 0.0000020849, 0.0000016433, 0.0000012979, 0.0000007813,
                        0.0000010596, 0.0000004958, 0.0000008546, 0.0000008928, 0.0000005593, 0.0000009962, 0.0000006205,
                        0.0000006694, 0.0000004270, 0.0000004437, 0.0000003712, 0.0000005299, 0.0000003383, 0.0000004823,
                        0.0000002425, 0.0000004443, 0.0000004843, 0.0000004110, 0.0000001667, 0.0000001831, 0.0000001992,
                        0.0000002251, 0.0000003165, 0.0000001686},
                {0.0000389225, 0.0000356628, 0.0000246098, 0.0000234810, 0.0000158826, 0.0000154517, 0.0000109594,
                        0.0000061715, 0.0000077203, 0.0000057494, 0.0000072006, 0.0000031528, 0.0000032123, 0.0000024479,
                        0.0000023896, 0.0000023499, 0.0000018589, 0.0000010985, 0.0000014288, 0.0000011269, 0.0000015138,
                        0.0000011720, 0.0000009382, 0.0000009879, 0.0000006065, 0.0000008389, 0.0000005190, 0.0000007012,
                        0.0000005724, 0.0000004774, 0.0000003925, 0.0000005365, 0.0000002703, 0.0000004855, 0.0000004093,
                        0.0000003496, 0.0000003709, 0.0000002481, 0.0000002089, 0.0000001810, 0.0000001978, 0.0000001719,
                        0.0000003092, 0.0000001622, 0.0000002294},
                {0.0000337803, 0.0000404714, 0.0000284070, 0.0000198125, 0.0000183754, 0.0000174965, 0.0000126377,
                        0.0000121499, 0.0000086656, 0.0000051637, 0.0000063642, 0.0000036800, 0.0000027442, 0.0000046261,
                        0.0000027739, 0.0000020991, 0.0000027411, 0.0000015953, 0.0000016454, 0.0000010057, 0.0000013415,
                        0.0000010441, 0.0000008211, 0.0000011139, 0.0000004134, 0.0000004295, 0.0000005796, 0.0000003604,
                        0.0000004939, 0.0000003153, 0.0000004360, 0.0000004611, 0.0000003900, 0.0000005448, 0.0000003467,
                        0.0000004915, 0.0000002450, 0.0000002082, 0.0000002350, 0.0000002486, 0.0000002168, 0.0000002377,
                        0.0000001597, 0.0000002236, 0.0000001948},
                {0.0000513067, 0.0000471119, 0.0000328458, 0.0000132586, 0.0000211621, 0.0000154592, 0.0000085830,
                        0.0000106377, 0.0000060140, 0.0000058815, 0.0000073820, 0.0000042029, 0.0000054567, 0.0000041212,
                        0.0000018646, 0.0000023994, 0.0000014275, 0.0000018493, 0.0000011151, 0.0000011543, 0.0000009245,
                        0.0000012066, 0.0000007246, 0.0000007519, 0.0000004803, 0.0000008191, 0.0000005011, 0.0000004138,
                        0.0000004368, 0.0000003569, 0.0000006387, 0.0000005261, 0.0000004315, 0.0000002781, 0.0000003871,
                        0.0000001946, 0.0000003503, 0.0000002314, 0.0000002482, 0.0000002172, 0.0000003055, 0.0000001223,
                        0.0000002858, 0.0000001905, 0.0000002117},
                {0.0000450816, 0.0000408154, 0.0000282845, 0.0000155178, 0.0000138761, 0.0000175937, 0.0000098262,
                        0.0000121258, 0.0000069414, 0.0000066229, 0.0000029233, 0.0000048194, 0.0000037116, 0.0000036148,
                        0.0000027563, 0.0000016149, 0.0000009741, 0.0000012561, 0.0000012827, 0.0000016831, 0.0000010254,
                        0.0000010378, 0.0000014009, 0.0000008403, 0.0000008877, 0.0000009243, 0.0000009675, 0.0000003626,
                        0.0000004818, 0.0000003920, 0.0000003302, 0.0000004548, 0.0000002250, 0.0000003083, 0.0000003321,
                        0.0000003601, 0.0000001838, 0.0000002508, 0.0000001319, 0.0000003035, 0.0000000966, 0.0000001703,
                        0.0000002389, 0.0000002086, 0.0000000873},
                {0.0000516615, 0.0000359046, 0.0000250255, 0.0000176967, 0.0000162043, 0.0000117023, 0.0000111500,
                        0.0000083372, 0.0000080845, 0.0000057760, 0.0000057683, 0.0000042198, 0.0000042307, 0.0000031731,
                        0.0000018613, 0.0000018786, 0.0000018362, 0.0000018880, 0.0000011419, 0.0000019392, 0.0000015208,
                        0.0000005532, 0.0000005717, 0.0000005781, 0.0000007821, 0.0000004847, 0.0000005016, 0.0000004026,
                        0.0000005570, 0.0000007553, 0.0000003651, 0.0000002375, 0.0000004137, 0.0000004454, 0.0000003721,
                        0.0000003077, 0.0000002562, 0.0000001692, 0.0000001437, 0.0000003380, 0.0000001705, 0.0000002396,
                        0.0000001596, 0.0000002284, 0.0000001911},
                {0.0000345441, 0.0000316241, 0.0000286584, 0.0000206648, 0.0000186795, 0.0000136872, 0.0000097047,
                        0.0000096665, 0.0000091419, 0.0000067531, 0.0000038985, 0.0000048445, 0.0000028978, 0.0000037146,
                        0.0000021550, 0.0000021547, 0.0000016273, 0.0000009960, 0.0000012967, 0.0000013308, 0.0000008052,
                        0.0000010640, 0.0000008348, 0.0000008528, 0.0000008847, 0.0000003357, 0.0000004374, 0.0000005905,
                        0.0000002889, 0.0000005062, 0.0000005427, 0.0000003357, 0.0000003624, 0.0000002976, 0.0000004179,
                        0.0000002635, 0.0000003711, 0.0000001854, 0.0000003373, 0.0000001698, 0.0000002400, 0.0000001599,
                        0.0000001739, 0.0000001177, 0.0000002124},
                {0.0000408929, 0.0000365974, 0.0000338537, 0.0000239837, 0.0000219789, 0.0000159079, 0.0000148083,
                        0.0000064093, 0.0000047381, 0.0000060372, 0.0000058068, 0.0000056862, 0.0000032968, 0.0000024911,
                        0.0000024593, 0.0000019073, 0.0000019128, 0.0000019005, 0.0000014645, 0.0000009027, 0.0000008974,
                        0.0000012109, 0.0000007360, 0.0000005796, 0.0000007748, 0.0000003802, 0.0000005085, 0.0000004071,
                        0.0000004279, 0.0000004439, 0.0000003642, 0.0000002963, 0.0000003129, 0.0000003366, 0.0000003574,
                        0.0000001793, 0.0000001519, 0.0000002667, 0.0000001760, 0.0000002482, 0.0000001261, 0.0000002263,
                        0.0000001493, 0.0000002143, 0.0000000869},
                {0.0000460894, 0.0000423271, 0.0000297879, 0.0000205531, 0.0000193030, 0.0000178901, 0.0000130914,
                        0.0000096919, 0.0000091541, 0.0000052562, 0.0000039234, 0.0000038485, 0.0000029218, 0.0000028777,
                        0.0000021609, 0.0000021577, 0.0000022213, 0.0000012807, 0.0000010154, 0.0000007967, 0.0000006264,
                        0.0000008077, 0.0000006542, 0.0000006707, 0.0000006786, 0.0000005617, 0.0000004357, 0.0000005927,
                        0.0000004775, 0.0000003001, 0.0000003137, 0.0000002632, 0.0000002739, 0.0000002881, 0.0000003141,
                        0.0000002009, 0.0000002152, 0.0000003873, 0.0000001959, 0.0000001287, 0.0000002305, 0.0000001938,
                        0.0000002115, 0.0000003004, 0.0000001533}
        };

    const float Bij[NUM_NECK_POINTS][NUM_NECK_POINTS] = {
            {-1.20, -1.05, -1.30, -1.15, -1.00, -0.85, -0.90, -0.75, -1.20, -1.05, -0.90, -0.95, -0.80, -0.65, -0.70,
             -0.75, -0.60, -0.45, -0.50, -0.75, -0.40, -0.25, -0.30, -0.15, -0.20, -0.25, 0.10,  0.05, 0.20,
             -0.05, 0.30, 0.25, 0.40, 0.15,  0.10, 0.25, 0.60, 0.55, 0.70, 0.45, 0.80, 0.55, 0.90, 0.65, 1.00},
            {-1.25, -1.10, -0.95, -1.40, -1.05, -1.10, -0.95, -0.80, -0.85, -0.70, -0.55, -1.00, -0.65, -0.70, -0.95,
             -0.60, -0.65, -0.70, -0.55, -0.60, -0.45, -0.10, -0.15, 0.00,  -0.05, 0.10,  0.05,  0.20, 0.15, 0.10,
             0.25, 0.00, 0.15, -0.10, 0.25, 0.20, 0.35, 0.50, 0.65, 0.40, 0.75, 0.70, 0.85, 0.60, 0.95},
            {-1.30, -1.15, -1.00, -1.05, -0.90, -0.95, -1.20, -0.85, -0.90, -0.95, -0.80, -0.65, -0.70, -0.75, -0.40,
             -0.65, -0.30, -0.35, -0.60, -0.45, -0.50, -0.35, -0.20, -0.25, -0.10, 0.05,  0.00,  0.15, -0.10,
             0.25,  0.00, 0.35, 0.10, 0.45,  0.40, 0.55, 0.30, 0.65, 0.60, 0.75, 0.50, 0.85, 0.80, 0.95, 0.70},
            {-1.15, -1.40, -1.05, -0.90, -1.15, -0.80, -1.05, -0.90, -0.95, -0.60, -0.85, -0.70, -0.75, -0.40, -0.45,
             -0.30, -0.35, -0.40, -0.25, -0.30, -0.15, -0.40, -0.05, -0.30, -0.15, -0.20, -0.05, 0.10, 0.25,
             0.00,  0.35, 0.10, 0.25, 0.20,  0.35, 0.30, 0.45, 0.40, 0.75, 0.50, 0.65, 0.80, 0.75, 0.90, 0.65},
            {-1.00, -1.25, -0.90, -1.15, -1.00, -0.85, -0.90, -0.75, -0.60, -0.85, -0.50, -0.95, -0.80, -0.65, -0.70,
             -0.55, -0.60, -0.25, -0.50, -0.35, -0.40, -0.05, -0.10, 0.05,  -0.20, 0.15,  -0.10, 0.05, 0.00,
             0.15,  0.10, 0.25, 0.40, 0.35,  0.50, 0.45, 0.60, 0.55, 0.70, 0.45, 0.80, 0.75, 0.90, 0.65, 1.00},
            {-0.85, -1.10, -1.15, -1.00, -0.85, -0.90, -0.95, -0.80, -0.65, -0.50, -0.55, -0.60, -0.65, -0.50, -0.55,
             -0.40, -0.25, -0.50, -0.15, -0.60, -0.05, -0.30, -0.15, 0.00,  -0.05, 0.10,  0.05,  0.20, 0.15, 0.30,
             0.05, 0.40, 0.35, 0.50,  0.45, 0.60, 0.55, 0.70, 0.45, 0.80, 0.75, 0.90, 0.85, 1.00, 0.75},
            {-0.90, -1.15, -1.00, -1.05, -0.90, -0.55, -0.60, -0.45, -0.70, -0.55, -0.60, -0.65, -0.50, -0.35, -0.40,
             -0.25, -0.50, -0.35, -0.20, -0.05, -0.10, 0.05,  0.00,  0.15,  0.30,  0.25,  0.40,  0.15, 0.30, -0.15,
             0.40, 0.55, 0.50, 0.25,  0.60, 0.55, 0.90, 0.45, 0.60, 0.75, 0.70, 0.85, 1.00, 0.95, 0.90},
            {-1.15, -1.00, -1.25, -0.90, -0.75, -0.80, -0.45, -0.50, -0.95, -0.40, -0.45, -0.70, -0.55, -0.40, -0.45,
             -0.30, -0.15, -0.20, -0.45, -0.10, -0.15, -0.20, -0.05, 0.10,  0.05,  0.20,  0.15,  0.10, 0.05, 0.40,
             0.35, 0.50, 0.45, 0.20,  0.55, 0.50, 0.45, 0.20, 0.75, 0.70, 0.85, 0.80, 0.95, 1.10, 1.05},
            {-1.00, -0.85, -0.90, -0.75, -0.80, -0.85, -0.90, -0.55, -0.60, -0.45, -0.50, -0.35, -0.40, -0.65, -0.30,
             -0.35, 0.00,  -0.25, 0.10,  -0.35, 0.00,  -0.05, -0.10, 0.05,  0.00,  0.15,  0.10,  0.25, 0.20, 0.15,
             0.30, 0.45, 0.00, 0.55,  0.70, 0.65, 0.80, 0.75, 0.90, 1.05, 1.00, 1.15, 1.10, 1.25, 1.00},
            {-1.05, -1.10, -0.75, -0.60, -0.65, -0.70, -0.95, -0.60, -0.45, -0.70, -0.55, -0.20, -0.45, -0.30, -0.35,
             -0.20, -0.25, -0.10, 0.05,  0.00,  -0.05, 0.30,  0.25,  0.00,  0.15,  0.10,  0.25,  0.20, 0.55, 0.50,
             0.45, 0.20, 0.75, 0.30,  0.85, 0.60, 0.55, 0.70, 0.85, 0.80, 0.75, 0.90, 1.05, 1.00, 1.35},
            {-1.10, -0.75, -0.80, -0.65, -0.50, -0.75, -0.60, -0.45, -0.70, -0.55, -0.40, -0.65, -0.30, -0.15, -0.20,
             -0.25, -0.30, -0.15, -0.20, -0.05, -0.10, 0.05,  0.20,  0.15,  0.30,  0.05,  0.20,  0.55, 0.30, 0.45,
             0.40, 0.55, 0.50, 0.65,  0.60, 0.55, 0.90, 0.65, 1.00, 0.95, 0.90, 0.65, 1.00, 0.75, 1.10},
            {-0.75, -1.00, -0.65, -0.50, -0.75, -0.60, -0.65, -0.30, -0.35, -0.20, -0.65, -0.50, -0.55, -0.20, -0.05,
             0.10,  -0.15, -0.20, -0.25, -0.10, -0.15, 0.00,  0.15,  0.10,  0.25,  0.20,  0.35,  0.30, 0.25, 0.40,
             0.55, 0.70, 0.45, 0.60,  0.55, 0.90, 0.85, 1.00, 0.75, 1.10, 1.05, 1.20, 1.15, 1.30, 1.05},
            {-0.80, -0.65, -0.70, -0.75, -0.60, -0.65, -0.50, -0.55, -0.40, -0.65, -0.30, -0.55, -0.40, -0.45,
             -0.10, -0.35, 0.00,  0.15,  0.10,  0.05,  -0.20, 0.35,  -0.10, 0.05,  0.20,  0.35,  0.50,  0.45, 0.40,
             0.75,  0.50, 0.85, 0.60, 0.55,  0.70, 0.85, 1.00, 0.75, 1.10, 1.05, 1.00, 1.15, 0.90, 1.05, 1.00},
            {-0.85, -0.70, -0.95, -0.60, -0.85, -0.50, -0.75, -0.40, -0.25, -0.30, -0.15, -0.20, -0.25, -0.10, 0.05,
             -0.20, -0.05, 0.10,  0.05,  -0.40, 0.35,  0.30,  0.45,  0.20,  -0.05, 0.10,  0.25,  0.40, 0.35, 0.50,
             0.65, 1.00, 0.75, 0.90,  0.65, 0.20, 0.75, 0.70, 0.85, 1.00, 0.95, 1.10, 1.25, 1.20, 1.35},
            {-0.70, -0.95, -0.60, -0.65, -0.50, -0.55, -0.40, -0.25, -0.30, -0.15, -0.20, -0.05, -0.30, 0.05,  0.20,
             -0.05, -0.30, 0.05,  0.00,  0.15,  0.10,  0.25,  0.20,  0.35,  0.10,  0.45,  0.60,  0.55, 0.50, 0.25,  0.20,
             0.55, 0.70, 0.85,  0.80, 1.15, 0.90, 1.25, 0.80, 0.95, 0.90, 1.05, 1.00, 1.55, 0.90},
            {-0.35, -0.80, -0.45, -0.50, -0.35, -0.40, -0.25, -0.50, -0.15, -0.20, -0.05, -0.50, -0.15, -0.40, -0.05,
             -0.10, 0.25,  0.20,  0.15,  0.10,  0.25,  0.20,  0.35,  0.30,  0.45,  0.40,  0.35,  0.70, 0.65, 0.40,  0.75,
             0.50, 0.65, 0.60,  0.95, 0.90, 1.05, 1.00, 1.15, 0.70, 1.25, 1.00, 0.95, 1.10, 1.05},
            {-0.40, -0.45, -0.50, -0.35, -0.60, -0.45, -0.50, -0.15, 0.00,  -0.25, -0.30, -0.15, -0.40, -0.05, 0.10,
             0.25,  0.40,  0.15,  0.10,  0.05,  0.40,  -0.05, 0.10,  0.25,  0.40,  0.35,  0.50,  0.45, 0.60, 0.75,  0.90,
             0.85, 0.60, 0.95,  0.70, 1.05, 0.80, 0.95, 0.90, 1.05, 1.20, 1.15, 1.30, 1.25, 1.40},
            {-0.45, -0.30, -0.35, -0.40, -0.25, -0.10, -0.35, -0.20, -0.25, -0.30, -0.15, 0.00,  -0.05, 0.10,  0.25,
             0.20,  0.15,  0.30,  0.05,  0.20,  0.35,  0.50,  0.65,  0.20,  0.55,  0.50,  0.85,  0.40, 0.75, 0.50,  0.65,
             0.60, 0.75, 0.90,  1.05, 1.20, 0.95, 1.30, 1.05, 1.00, 0.95, 1.30, 1.25, 1.40, 0.95},
            {-0.30, -0.35, -0.40, -0.45, -0.30, -0.35, -0.20, -0.45, -0.10, 0.05,  0.20,  -0.05, -0.10, -0.15, 0.20,
             0.15,  0.10,  0.05,  0.60,  0.35,  0.50,  0.25,  0.40,  0.55,  0.70,  0.65,  0.80,  0.55, 0.70, 1.05,  1.00,
             0.55, 0.90, 1.05,  1.20, 0.75, 1.10, 0.85, 1.40, 1.35, 1.50, 1.05, 1.20, 1.15, 1.30},
            {-0.55, -0.40, -0.65, -0.30, -0.35, -0.20, -0.25, -0.10, -0.35, 0.00,  -0.05, 0.10,  0.05,  0.00,  -0.25,
             0.30,  0.25,  0.20,  0.35,  -0.10, 0.25,  0.40,  0.55,  0.70,  0.65,  0.60,  0.75,  0.90, 0.65, 0.40,  0.95,
             0.70, 1.05, 1.00,  1.15, 0.90, 1.05, 1.00, 0.75, 1.30, 1.25, 1.60, 1.35, 1.10, 1.45},
            {-0.40, -0.45, -0.70, -0.15, -0.20, -0.05, 0.10,  -0.35, 0.00,  0.15,  -0.10, 0.25,  0.00,  0.35,  0.30,  0.25,
             0.20,  0.55,  0.50,  0.45,  0.20,  0.75,  0.50,  0.65,  0.60,  0.75,  0.50,  0.85, 0.80, 0.95,  1.10, 1.05,
             1.00, 1.15,  1.10, 1.05, 1.20, 0.95, 1.10, 1.25, 1.20, 1.35, 1.30, 1.45, 1.40},
            {-0.25, -0.10, -0.15, -0.40, -0.45, -0.30, 0.05,  -0.20, 0.15,  0.10,  0.05,  0.00,  0.35,  0.10,  0.45,  0.20,
             0.35,  0.10,  0.25,  0.40,  0.55,  0.30,  0.65,  0.20,  0.35,  0.30,  0.65,  0.60, 0.75, 0.90,  0.85, 1.00,
             0.95, 1.30,  1.05, 1.20, 1.15, 1.10, 1.05, 1.20, 1.35, 1.30, 1.45, 1.40, 1.75},
            {-0.30, -0.15, -0.20, -0.25, 0.10,  -0.35, -0.20, 0.15,  -0.10, 0.05,  0.40,  0.15,  0.10,  0.45,  0.00,  0.35,
             0.10,  0.45,  0.60,  0.55,  0.50,  0.65,  0.20,  0.35,  0.90,  0.45,  0.60,  0.35, 0.90, 0.65,  0.60, 0.95,
             1.10, 1.25,  1.00, 1.15, 1.30, 1.45, 1.00, 1.15, 1.50, 1.65, 1.20, 1.35, 1.30},
            {-0.15, -0.20, -0.05, -0.10, -0.15, -0.40, -0.05, 0.10,  0.05,  0.00,  0.15,  0.30,  0.25,  0.20,  0.55,  0.50,
             0.45,  0.20,  0.15,  0.70,  0.45,  0.60,  0.35,  0.90,  0.65,  0.80,  0.55,  0.50, 0.45, 0.80,  0.95, 0.90,
             1.05, 1.00,  0.95, 1.10, 1.25, 1.40, 1.35, 1.30, 1.45, 1.20, 1.55, 1.70, 1.65},
            {0.00,  -0.25, -0.10, -0.15, 0.00,  -0.05, -0.10, 0.05,  0.00,  0.15,  0.30,  0.25,  0.20,  0.15,  0.50,  0.45,
             0.20,  0.35,  0.70,  0.45,  0.40,  0.35,  0.70,  0.65,  0.80,  0.75,  0.90,  0.65, 1.00, 0.95,  1.30, 1.25,
             1.00, 1.35,  0.90, 1.25, 1.00, 1.35, 1.30, 1.45, 1.40, 1.55, 1.70, 1.25, 1.60},
            {-0.25, -0.10, -0.15, 0.00,  0.15,  0.10,  -0.15, 0.20,  0.15,  0.10,  0.05,  0.40,  0.35,  0.50,  0.05,  0.00,
             0.35,  0.50,  0.25,  0.60,  0.35,  0.50,  0.65,  1.00,  0.95,  0.90,  0.85,  0.80, 1.15, 1.10,  1.25, 1.40,
             1.15, 0.90,  1.05, 1.60, 1.55, 1.10, 1.25, 1.60, 1.35, 1.30, 1.45, 1.60, 1.55},
            {-0.10, -0.15, 0.00,  -0.05, 0.10,  -0.15, 0.00,  0.35,  0.30,  0.05,  0.20,  0.55,  0.30,  0.65,  0.60,  0.15,  0.50,
             0.25,  0.60,  0.55,  0.90,  0.85,  0.60,  0.55,  0.70,  1.05,  0.80,  0.95, 0.90, 1.05,  1.00, 1.15, 1.30,
             1.25,  1.20, 1.55, 0.90, 1.45, 1.40, 1.35, 1.90, 1.45, 1.60, 1.55, 1.50},
            {0.05,  0.00,  0.15,  0.10,  -0.15, 0.20,  0.35,  0.10,  0.45,  0.20,  0.15,  0.30,  0.05,  0.40,  0.15,  0.70,  0.45,
             0.40,  0.75,  0.50,  0.85,  0.60,  0.75,  0.90,  1.05,  1.20,  1.15,  0.90, 1.25, 1.00,  0.95, 1.10, 1.25,
             1.00,  1.15, 1.10, 1.25, 1.60, 1.15, 1.30, 1.25, 1.20, 1.75, 1.50, 1.65},
            {0.20,  0.15,  -0.10, -0.15, 0.00,  0.15,  0.10,  0.45,  0.20,  0.35,  0.10,  0.45,  0.40,  0.35,  0.30,  0.65,  0.40,
             0.35,  0.70,  0.85,  0.60,  0.55,  0.50,  0.85,  1.20,  0.95,  0.50,  0.85, 1.20, 1.35,  0.90, 1.05, 1.40,
             1.35,  1.50, 1.25, 1.40, 1.55, 1.70, 1.05, 1.60, 1.75, 1.70, 1.65, 1.80},
            {0.15,  0.10,  0.05,  0.00,  0.15,  0.10,  0.25,  0.60,  0.35,  0.30,  0.05,  0.60,  0.55,  0.30,  0.25,  0.40,  0.55,
             0.50,  0.85,  0.40,  0.75,  1.10,  1.05,  0.40,  0.95,  0.70,  0.85,  1.00, 0.95, 1.30,  1.05, 1.40, 1.35,
             1.70,  1.45, 1.40, 1.35, 1.50, 1.45, 1.60, 1.55, 1.70, 1.85, 2.00, 1.75},
            {0.10,  0.05,  0.00,  0.15,  0.10,  0.25,  0.20,  0.35,  0.10,  0.45,  0.60,  0.55,  0.10,  0.65,  0.80,  0.75,  0.30,
             0.65,  0.40,  0.75,  1.10,  0.85,  0.40,  0.75,  0.70,  0.85,  1.20,  0.95, 1.30, 1.25,  1.40, 0.95, 1.50,
             0.85,  1.60, 1.75, 1.70, 0.65, 1.60, 1.55, 1.50, 1.65, 1.80, 1.75, 1.90},
            {0.05,  0.20,  0.15,  0.30,  0.25,  0.40,  0.35,  0.50,  0.65,  0.20,  0.55,  0.30,  0.45,  0.40,  0.55,  0.90,  0.85,
             0.40,  0.55,  0.90,  0.65,  0.80,  0.95,  1.10,  1.45,  1.40,  0.95,  1.10, 1.05, 1.00,  1.35, 1.30, 1.05,
             1.40,  1.75, 1.30, 1.65, 1.40, 1.75, 1.50, 2.05, 1.60, 2.15, 1.90, 1.65},
            {0.20,  0.15,  0.30,  0.25,  0.40,  0.35,  0.30,  0.25,  0.60,  0.35,  0.70,  0.85,  0.20,  0.95,  0.70,  0.65,  0.80,
             0.55,  0.90,  0.85,  1.00,  0.95,  0.90,  1.05,  1.20,  1.35,  1.10,  1.25, 0.80, 1.55,  1.30, 1.25, 1.20,
             1.95,  1.50, 1.45, 1.20, 1.95, 1.30, 1.85, 1.40, 1.95, 1.50, 1.85, 1.80},
            {0.35,  0.50,  0.45,  0.40,  0.55,  0.50,  0.45,  0.60,  0.55,  0.50,  0.45,  0.60,  0.55,  0.70,  0.85,  0.80,  0.75,
             0.70,  1.05,  1.00,  0.55,  0.90,  1.25,  1.00,  1.35,  1.10,  1.45,  1.00, 1.75, 1.50,  1.45, 1.40, 1.75,
             1.30,  1.25, 1.60, 1.95, 1.10, 1.85, 2.00, 1.95, 2.10, 1.85, 2.20, 2.15},
            {0.30,  0.45,  0.40,  0.15,  0.50,  0.65,  0.40,  0.35,  0.50,  0.85,  0.80,  0.75,  0.70,  0.85,  0.80,  0.95,  0.70,
             1.05,  0.80,  1.15,  0.90,  1.05,  1.00,  1.35,  0.90,  1.05,  1.00,  1.35, 1.10, 1.45,  1.40, 1.75, 1.70,
             1.65,  1.40, 1.35, 1.30, 1.85, 2.20, 1.55, 1.90, 1.85, 2.00, 1.55, 2.10},
            {0.45,  0.20,  0.15,  0.30,  0.45,  0.40,  0.55,  0.50,  0.65,  0.60,  0.75,  0.70,  0.85,  0.60,  0.95,  0.70,  1.05,
             0.80,  0.95,  0.90,  1.05,  0.80,  1.15,  1.10,  1.25,  1.60,  0.75,  1.10, 1.25, 1.40,  1.55, 1.70, 1.45,
             1.60,  1.75, 1.90, 1.65, 1.80, 2.15, 2.10, 1.45, 2.00, 1.75, 1.90, 1.85},
            {0.20,  0.35,  0.30,  0.45,  0.40,  0.55,  0.50,  0.45,  0.80,  0.75,  0.50,  0.85,  1.00,  0.75,  0.50,  0.85,  1.20,
             1.15,  1.10,  1.05,  0.80,  1.15,  0.70,  1.25,  1.40,  1.15,  1.70,  1.45, 1.60, 1.35,  1.50, 1.45, 1.80,
             1.55,  1.90, 1.45, 2.00, 2.15, 2.10, 1.45, 1.60, 1.75, 1.90, 2.25, 1.80},
            {0.35,  0.50,  0.45,  0.60,  0.55,  0.70,  0.65,  0.40,  0.75,  0.70,  1.05,  0.60,  0.75,  0.70,  0.85,  1.00,  0.95,
             0.70,  1.05,  1.00,  1.35,  1.30,  1.25,  1.40,  1.15,  1.50,  1.25,  1.60, 1.55, 1.50,  1.45, 1.80, 1.35,
             1.90,  1.85, 1.80, 1.95, 1.70, 1.65, 1.60, 1.75, 1.70, 2.25, 1.80, 2.15},
            {0.30,  0.65,  0.60,  0.55,  0.70,  0.85,  0.80,  0.95,  0.90,  0.65,  1.00,  0.75,  0.70,  1.25,  1.00,  0.95,  1.30,
             1.05,  1.20,  0.95,  1.30,  1.25,  1.20,  1.55,  0.90,  1.05,  1.40,  1.15, 1.50, 1.25,  1.60, 1.75, 1.70,
             2.05,  1.80, 2.15, 1.70, 1.65, 1.80, 1.95, 1.90, 2.05, 1.80, 2.15, 2.10},
            {0.65,  0.80,  0.75,  0.30,  0.85,  0.80,  0.55,  0.90,  0.65,  0.80,  1.15,  0.90,  1.25,  1.20,  0.75,  1.10,  0.85,
             1.20,  0.95,  1.10,  1.05,  1.40,  1.15,  1.30,  1.05,  1.60,  1.35,  1.30, 1.45, 1.40,  1.95, 1.90, 1.85,
             1.60,  1.95, 1.50, 2.05, 1.80, 1.95, 1.90, 2.25, 1.60, 2.35, 2.10, 2.25},
            {0.60,  0.75,  0.70,  0.45,  0.60,  0.95,  0.70,  1.05,  0.80,  0.95,  0.50,  1.05,  1.00,  1.15,  1.10,  0.85,  0.60,
             0.95,  1.10,  1.45,  1.20,  1.35,  1.70,  1.45,  1.60,  1.75,  1.90,  1.25, 1.60, 1.55,  1.50, 1.85, 1.40,
             1.75,  1.90, 2.05, 1.60, 1.95, 1.50, 2.25, 1.40, 1.95, 2.30, 2.25, 1.60},
            {0.75,  0.70,  0.65,  0.60,  0.75,  0.70,  0.85,  0.80,  0.95,  0.90,  1.05,  1.00,  1.15,  1.10,  0.85,  1.00,  1.15,
             1.30,  1.05,  1.60,  1.55,  0.90,  1.05,  1.20,  1.55,  1.30,  1.45,  1.40, 1.75, 2.10,  1.65, 1.40, 1.95,
             2.10,  2.05, 2.00, 1.95, 1.70, 1.65, 2.40, 1.95, 2.30, 2.05, 2.40, 2.35},
            {0.50,  0.65,  0.80,  0.75,  0.90,  0.85,  0.80,  0.95,  1.10,  1.05,  0.80,  1.15,  0.90,  1.25,  1.00,  1.15,  1.10,
             0.85,  1.20,  1.35,  1.10,  1.45,  1.40,  1.55,  1.70,  1.05,  1.40,  1.75, 1.30, 1.85,  2.00, 1.75, 1.90,
             1.85,  2.20, 1.95, 2.30, 1.85, 2.40, 1.95, 2.30, 2.05, 2.20, 1.95, 2.50},
            {0.65,  0.80,  0.95,  0.90,  1.05,  1.00,  1.15,  0.70,  0.65,  1.00,  1.15,  1.30,  1.05,  1.00,  1.15,  1.10,  1.25,
             1.40,  1.35,  1.10,  1.25,  1.60,  1.35,  1.30,  1.65,  1.20,  1.55,  1.50, 1.65, 1.80,  1.75, 1.70, 1.85,
             2.00,  2.15, 1.70, 1.65, 2.20, 1.95, 2.30, 1.85, 2.40, 2.15, 2.50, 1.85},
            {0.80,  0.95,  0.90,  0.85,  1.00,  1.15,  1.10,  1.05,  1.20,  0.95,  0.90,  1.05,  1.00,  1.15,  1.10,  1.25,  1.40,
             1.15,  1.10,  1.05,  1.00,  1.35,  1.30,  1.45,  1.60,  1.55,  1.50,  1.85, 1.80, 1.55,  1.70, 1.65, 1.80,
             1.95,  2.10, 1.85, 2.00, 2.55, 2.10, 1.85, 2.40, 2.35, 2.50, 2.85, 2.40}
    };

private:
    class ForceInfo;
    ComputeContext& cc;
    const System& system;
    bool includeSurfaceArea, tanhRescaling, hasInitializedKernels;
    int computeBornSumThreads, gkForceThreads, chainRuleThreads, ediffThreads;
    AmoebaMultipoleForce::PolarizationType polarizationType;
    std::map<std::string, std::string> defines;
    ComputeArray params;
    ComputeArray neckRadii;
    ComputeArray neckA;
    ComputeArray neckB;
    ComputeArray bornSum;
    ComputeArray bornRadii;
    ComputeArray bornForce;
    ComputeArray field;
    ComputeArray inducedField;
    ComputeArray inducedFieldPolar;
    ComputeArray inducedDipoleS;
    ComputeArray inducedDipolePolarS;
    ComputeKernel computeBornSumKernel, reduceBornSumKernel, surfaceAreaKernel, gkForceKernel, chainRuleKernel, ediffKernel;
};

/**
 * This kernel is invoked to calculate the vdw forces acting on the system and the energy of the system.
 */
class CommonCalcAmoebaVdwForceKernel : public CalcAmoebaVdwForceKernel {
public:
    CommonCalcAmoebaVdwForceKernel(const std::string& name, const Platform& platform, ComputeContext& cc, const System& system);
    ~CommonCalcAmoebaVdwForceKernel();
    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the AmoebaVdwForce this kernel will be used for
     */
    void initialize(const System& system, const AmoebaVdwForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    /**
     * Copy changed parameters over to a context.
     *
     * @param context    the context to copy parameters to
     * @param force      the AmoebaVdwForce to copy the parameters from
     */
    void copyParametersToContext(ContextImpl& context, const AmoebaVdwForce& force);
private:
    class ForceInfo;
    ComputeContext& cc;
    const System& system;
    bool hasInitializedNonbonded;

    // True if the AmoebaVdwForce AlchemicalMethod is not None.
    bool hasAlchemical;
    // Device memory for the alchemical state.
    ComputeArray vdwLambda;
    // Only update device memory when lambda changes.
    float currentVdwLambda;
    // Per particle alchemical flag.
    ComputeArray isAlchemical;
    // Per particle scale factor.
    ComputeArray scaleFactors;

    double dispersionCoefficient;
    ComputeArray sigmaEpsilon, atomType;
    ComputeArray bondReductionAtoms;
    ComputeArray bondReductionFactors;
    ComputeArray tempPosq;
    ComputeArray tempForces;
    NonbondedUtilities* nonbonded;
    ComputeKernel prepareKernel, spreadKernel;
};

/**
 * This kernel is invoked to calculate the WCA dispersion forces acting on the system and the energy of the system.
 */
class CommonCalcAmoebaWcaDispersionForceKernel : public CalcAmoebaWcaDispersionForceKernel {
public:
    CommonCalcAmoebaWcaDispersionForceKernel(const std::string& name, const Platform& platform, ComputeContext& cc, const System& system);
    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the AmoebaMultipoleForce this kernel will be used for
     */
    void initialize(const System& system, const AmoebaWcaDispersionForce& force);
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    /**
     * Copy changed parameters over to a context.
     *
     * @param context    the context to copy parameters to
     * @param force      the AmoebaWcaDispersionForce to copy the parameters from
     */
    void copyParametersToContext(ContextImpl& context, const AmoebaWcaDispersionForce& force);
private:
    class ForceInfo;
    ComputeContext& cc;
    const System& system;
    double totalMaximumDispersionEnergy;
    int forceThreadBlockSize;
    ComputeArray radiusEpsilon;
    ComputeKernel forceKernel;
};

/**
 * This kernel is invoked by HippoNonbondedForce to calculate the forces acting on the system and the energy of the system.
 */
class CommonCalcHippoNonbondedForceKernel : public CalcHippoNonbondedForceKernel {
public:
    CommonCalcHippoNonbondedForceKernel(const std::string& name, const Platform& platform, ComputeContext& cc, const System& system);
    /**
     * Initialize the kernel.
     * 
     * @param system     the System this kernel will be applied to
     * @param force      the HippoNonbondedForce this kernel will be used for
     */
    void initialize(const System& system, const HippoNonbondedForce& force);
    /**
     * Compute the FFT.
     */
    virtual void computeFFT(bool forward, bool dispersion) = 0;
    /**
     * Get whether charge spreading should be done in fixed point.
     */
    virtual bool useFixedPointChargeSpreading() const = 0;
    /**
     * Sort the atom grid indices.
     */
    virtual void sortGridIndex() = 0;
    /**
     * Execute the kernel to calculate the forces and/or energy.
     *
     * @param context        the context in which to execute this kernel
     * @param includeForces  true if forces should be calculated
     * @param includeEnergy  true if the energy should be calculated
     * @return the potential energy due to the force
     */
    double execute(ContextImpl& context, bool includeForces, bool includeEnergy);
    /**
     * Get the induced dipole moments of all particles.
     * 
     * @param context    the Context for which to get the induced dipoles
     * @param dipoles    the induced dipole moment of particle i is stored into the i'th element
     */
    void getInducedDipoles(ContextImpl& context, std::vector<Vec3>& dipoles);
    /**
     * Get the fixed dipole moments of all particles in the global reference frame.
     * 
     * @param context    the Context for which to get the fixed dipoles
     * @param dipoles    the fixed dipole moment of particle i is stored into the i'th element
     */
    void getLabFramePermanentDipoles(ContextImpl& context, std::vector<Vec3>& dipoles);
    /** 
     * Calculate the electrostatic potential given vector of grid coordinates.
     *
     * @param context                      context
     * @param inputGrid                    input grid coordinates
     * @param outputElectrostaticPotential output potential 
     */
    void getElectrostaticPotential(ContextImpl& context, const std::vector< Vec3 >& inputGrid,
                                   std::vector< double >& outputElectrostaticPotential);
    /**
     * Copy changed parameters over to a context.
     *
     * @param context    the context to copy parameters to
     * @param force      the HippoNonbondedForce to copy the parameters from
     */
    void copyParametersToContext(ContextImpl& context, const HippoNonbondedForce& force);
    /**
     * Get the parameters being used for PME.
     * 
     * @param alpha   the separation parameter
     * @param nx      the number of grid points along the X axis
     * @param ny      the number of grid points along the Y axis
     * @param nz      the number of grid points along the Z axis
     */
    void getPMEParameters(double& alpha, int& nx, int& ny, int& nz) const;
    /**
     * Get the parameters being used for dispersion PME.
     * 
     * @param alpha   the separation parameter
     * @param nx      the number of grid points along the X axis
     * @param ny      the number of grid points along the Y axis
     * @param nz      the number of grid points along the Z axis
     */
    void getDPMEParameters(double& alpha, int& nx, int& ny, int& nz) const;
protected:
    class ForceInfo;
    class TorquePostComputation;
    void computeInducedField(int optOrder);
    void computeExtrapolatedDipoles();
    void ensureMultipolesValid(ContextImpl& context);
    void addTorquesToForces();
    void createFieldKernel(const std::string& interactionSrc, std::vector<ComputeArray*> params, ComputeArray& fieldBuffer,
        ComputeKernel& kernel, ComputeKernel& exceptionKernel, ComputeArray& exceptionScale);
    int numParticles, maxExtrapolationOrder, maxTiles, fieldThreadBlockSize;
    int gridSizeX, gridSizeY, gridSizeZ;
    int dispersionGridSizeX, dispersionGridSizeY, dispersionGridSizeZ;
    double pmeAlpha, dpmeAlpha, cutoff;
    bool usePME, hasInitializedKernels, multipolesAreValid;
    std::vector<double> extrapolationCoefficients;
    ComputeContext& cc;
    const System& system;
    ComputeArray multipoleParticles;
    ComputeArray coreCharge, valenceCharge, alpha, epsilon, damping, c6, pauliK, pauliQ, pauliAlpha, polarizability;
    ComputeArray localDipoles, labDipoles, fracDipoles;
    ComputeArray localQuadrupoles, labQuadrupoles[5], fracQuadrupoles;
    ComputeArray field;
    ComputeArray inducedField;
    ComputeArray torque;
    ComputeArray inducedDipole;
    ComputeArray extrapolatedDipole, extrapolatedPhi;
    ComputeArray pmeGrid1, pmeGrid2, pmeGridLong;
    ComputeArray pmeAtomGridIndex;
    ComputeArray pmeBsplineModuliX, pmeBsplineModuliY, pmeBsplineModuliZ;
    ComputeArray dpmeBsplineModuliX, dpmeBsplineModuliY, dpmeBsplineModuliZ;
    ComputeArray pmePhi, pmePhidp, pmeCphi;
    ComputeArray lastPositions;
    ComputeArray exceptionScales[6];
    ComputeArray exceptionAtoms;
    ComputeKernel computeMomentsKernel, recordInducedDipolesKernel, mapTorqueKernel;
    ComputeKernel fixedFieldKernel, fixedFieldExceptionKernel, mutualFieldKernel, mutualFieldExceptionKernel, computeExceptionsKernel;
    ComputeKernel pmeSpreadFixedMultipolesKernel, pmeSpreadInducedDipolesKernel, pmeFinishSpreadChargeKernel, pmeConvolutionKernel;
    ComputeKernel pmeFixedPotentialKernel, pmeInducedPotentialKernel, pmeFixedForceKernel, pmeInducedForceKernel, pmeRecordInducedFieldDipolesKernel;
    ComputeKernel pmeSelfEnergyKernel, pmeTransformMultipolesKernel, pmeTransformPotentialKernel;
    ComputeKernel dpmeGridIndexKernel, dpmeSpreadChargeKernel, dpmeFinishSpreadChargeKernel, dpmeEvalEnergyKernel, dpmeConvolutionKernel, dpmeInterpolateForceKernel;
    ComputeKernel initExtrapolatedKernel, iterateExtrapolatedKernel, computeExtrapolatedKernel, polarizationEnergyKernel;
    static const int PmeOrder = 5;
};

} // namespace OpenMM

#endif /*AMOEBA_OPENMM_COMMONKERNELS_H_*/