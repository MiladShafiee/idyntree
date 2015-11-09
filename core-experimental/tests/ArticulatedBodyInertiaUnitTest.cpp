/*
 * Copyright (C) 2015 Fondazione Istituto Italiano di Tecnologia
 * Authors: Silvio Traversaro
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

#include <iDynTree/Core/EigenHelpers.h>
#include <iDynTree/Core/IMatrix.h>
#include <iDynTree/Core/ArticulatedBodyInertia.h>
#include <iDynTree/Core/SpatialInertia.h>
#include <iDynTree/Core/SpatialMomentum.h>
#include <iDynTree/Core/Transform.h>
#include <iDynTree/Core/Utils.h>
#include <iDynTree/Core/TestUtils.h>
#include <iDynTree/Core/Twist.h>
#include <iDynTree/Core/MatrixFixSize.h>

#include <Eigen/Dense>

#include <cstdio>
#include <cstdlib>

using namespace iDynTree;

void checkInertiaTransformation(const Transform & trans, const ArticulatedBodyInertia & inertia)
{
    ArticulatedBodyInertia inertiaTranslated = trans*inertia;
    Matrix6x6      inertiaTranslatedCheck;

    Matrix6x6 adjWre = trans.asAdjointTransformWrench();
    Matrix6x6 I      = inertia.asMatrix();
    Matrix6x6 adj    = trans.inverse().asAdjointTransform();
    toEigen(inertiaTranslatedCheck) = toEigen(adjWre)*
                                      toEigen(I)*
                                      toEigen(adj);

    Matrix6x6 inertiaTranslatedRaw = inertiaTranslated.asMatrix();

    std::cout << " inertiaTranslatedCheck : " << std::endl;
    std::cout << inertiaTranslatedCheck.toString() << std::endl;
    std::cout << " inertiaTranslated : " << std::endl;
    std::cout << inertiaTranslatedRaw.toString() << std::endl;

    ASSERT_EQUAL_MATRIX(inertiaTranslatedCheck,inertiaTranslatedRaw);

}

void checkInertiaAccProduct(const ArticulatedBodyInertia & inertia, const SpatialMotionVector & twist)
{
    SpatialForceVector momentum = inertia*twist;
    Vector6         momentumCheck;

    Matrix6x6 I = inertia.asMatrix();

    Vector6 twistPlain = twist.asVector();
    toEigen(momentumCheck) = toEigen(I)*toEigen(twistPlain);

    ASSERT_EQUAL_VECTOR(momentum.asVector(),momentumCheck);
}

void checkInvariance(const Transform & trans, const ArticulatedBodyInertia & inertia, const SpatialMotionVector & twist)
{
    Transform invTrans = trans.inverse();
    SpatialForceVector momentumTranslated = trans*(inertia*twist);
    SpatialForceVector momentumTranslatedCheck = (trans*inertia)*(trans*twist);

    SpatialMotionVector           twistTranslated         = trans*twist;
    ArticulatedBodyInertia  inertiaTranslated       = trans*inertia;
    Vector6 momentumTranslatedCheck2;
    Vector6 momentumTranslatedCheck3;
    Vector6 twistTranslatedCheck;
    Matrix6x6 transAdjWrench = trans.asAdjointTransformWrench();
    Matrix6x6 inertiaRaw     = inertia.asMatrix();
    Matrix6x6 transInvAdj    = trans.inverse().asAdjointTransform();
    Matrix6x6 transAdj       = trans.asAdjointTransform();
    Matrix6x6 inertiaTranslatedCheck;
    Vector6 twistPlain = twist.asVector();

    toEigen(momentumTranslatedCheck2) = toEigen(transAdjWrench)*
                                        toEigen(inertiaRaw)*
                                        toEigen(twistPlain);

    toEigen(momentumTranslatedCheck3) = toEigen(transAdjWrench)*
                                        toEigen(inertiaRaw)*
                                        toEigen(transInvAdj)*
                                        toEigen(transAdj)*
                                        toEigen(twistPlain);

    toEigen(twistTranslatedCheck)     = toEigen(transAdj)*
                                        toEigen(twistPlain);

    toEigen(inertiaTranslatedCheck)   = toEigen(transAdjWrench)*
                                        toEigen(inertiaRaw)*
                                        toEigen(transInvAdj);

    ASSERT_EQUAL_MATRIX(inertiaTranslatedCheck,inertiaTranslated.asMatrix());
    ASSERT_EQUAL_VECTOR(twistTranslated.asVector(),twistTranslatedCheck);
    ASSERT_EQUAL_VECTOR(momentumTranslated.asVector(),momentumTranslatedCheck3);
    ASSERT_EQUAL_VECTOR(momentumTranslated.asVector(),momentumTranslatedCheck2);
    ASSERT_EQUAL_VECTOR(momentumTranslated.asVector(),momentumTranslatedCheck.asVector());

    SpatialForceVector momentum = invTrans*momentumTranslated;
    SpatialForceVector momentumCheck = (invTrans*(trans*inertia))*(invTrans*(trans*twist));

    ASSERT_EQUAL_VECTOR(momentum.asVector(),momentumCheck.asVector());
}

int main()
{
    Transform trans(Rotation::RPY(4.0,5.0,6.0),Position(10,30,-30));

    double twistData[6] = {10.0,0.8,0.5,1.0,0.6,-6.0};
    SpatialMotionVector twist(LinVelocity(twistData,3),AngVelocity(twistData+3,3));

    ASSERT_EQUAL_DOUBLE(twist.asVector()(0),twistData[0]);

    double rotInertiaData[3*3] = {10.0,0.5,0.8,
                                  0.5,20.0,0.6,
                                  0.8,0.6,25.0};
    SpatialInertia inertia(1.0,Position(0.3,0.6,0.2),RotationalInertiaRaw(rotInertiaData,3,3));
    ArticulatedBodyInertia abi = ArticulatedBodyInertia(inertia);

    checkInertiaAccProduct(abi,twist);
    checkInertiaTransformation(trans,abi);
    checkInvariance(trans,abi,twist);

    return EXIT_SUCCESS;
}