/*
 * Copyright (C) 2014,2018 Fondazione Istituto Italiano di Tecnologia
 *
 * Licensed under either the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 * or the GNU Lesser General Public License v2.1 :
 * https://www.gnu.org/licenses/old-licenses/lgpl-2.1.html
 * at your option.
 *
 * Originally developed for Prioritized Optimal Control (2014)
 * Refactored in 2018.
 * Design inspired by
 * - ACADO toolbox (http://acado.github.io)
 * - ADRL Control Toolbox (https://adrlab.bitbucket.io/ct/ct_doc/doc/html/index.html)
 */

#ifndef IDYNTREE_OPTIMALCONTROL_LINEARCONSTRAINT_H
#define IDYNTREE_OPTIMALCONTROL_LINEARCONSTRAINT_H

#include <iDynTree/Constraint.h>
#include <string>
#include <iDynTree/Core/MatrixDynSize.h>

namespace iDynTree {
    namespace optimalcontrol {

        /*@
         * Models a linear constraints on the state and/or control, i.e.
         * \f[
         *      lb \leq A \begin{bmatrix} x\\u\end{bmatrix} \leq ub
         * \f]
         */

        /**
         * @warning This class is still in active development, and so API interface can change between iDynTree versions.
         * \ingroup iDynTreeExperimental
         */

        class LinearConstraint
        : public Constraint {
        public:

            LinearConstraint(size_t size, const std::string name);

            virtual ~LinearConstraint() override;

            bool setStateConstraintMatrix(const MatrixDynSize& constraintMatrix);

            bool setControlConstraintMatrix(const MatrixDynSize& constraintMatrix);

            virtual bool evaluateConstraint(double time,
                                            const VectorDynSize& state,
                                            const VectorDynSize& control,
                                            VectorDynSize& constraint) final;// lu <= [Ax, Au][x;u] <= lU

            virtual bool constraintJacobianWRTState(double time,
                                                    const VectorDynSize& state,
                                                    const VectorDynSize& control,
                                                    MatrixDynSize& jacobian) final;

            virtual bool constraintJacobianWRTControl(double time,
                                                      const VectorDynSize& state,
                                                      const VectorDynSize& control,
                                                      MatrixDynSize& jacobian) final;

        private:
            bool m_constrainsState, m_constrainsControl;
            iDynTree::MatrixDynSize m_stateConstraintMatrix;
            iDynTree::MatrixDynSize m_controlConstraintMatrix;
            iDynTree::VectorDynSize m_stateConstraintsBuffer, m_controlConstraintsBuffer;
        };

    }
}

#endif /* end of include guard: IDYNTREE_OPTIMALCONTROL_LINEARCONSTRAINT_H */