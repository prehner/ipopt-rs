// Copyright (C) 2004, 2010 International Business Machines and others.
// All Rights Reserved.
// This code is published under the Eclipse Public License.
//
// $Id: IpStdInterfaceTNLP.hpp 1861 2010-12-21 21:34:47Z andreasw $
//
// Authors:  Carl Laird, Andreas Waechter     IBM    2004-08-13
// Edited by: Egor Larionov (2018)

#ifndef __IPSTDINTERFACETNLP_HPP__
#define __IPSTDINTERFACETNLP_HPP__

#include "IpUtils.hpp"
#include "IpTNLP.hpp"
#include "IpJournalist.hpp"
#include "IpException.hpp"
#include "IpStdCInterface.h"
#include "IpSmartPtr.hpp"
#include <vector>

namespace Ipopt
{
  /** Declare excpetion that is thrown when invalid NLP data
   *  is provided */
  DECLARE_STD_EXCEPTION(INVALID_STDINTERFACE_NLP);

  /** Implementation of a TNLP for the Standard C interface.  The
   *  standard C interface is exposed to the user as a single C
   *  function that is given problem dimension, starting points, and
   *  pointers for functions that evaluate objective function etc.
   */
  class StdInterfaceTNLP : public TNLP
  {
  public:
    /**@name Constructors/Destructors */
    //@{
    /** Constructor, given dimensions of problem, function pointers
     *  for evaluation callback functions, and starting points. Note
     *  that the constrctor does not make a copy of any of the Number
     *  arrays, i.e. it is up to the called to keep them around. */
    StdInterfaceTNLP(Index index_style,
                     Sizes_CB sizes,
                     Init_CB init,
                     Bounds_CB bounds,
                     Eval_F_CB eval_f,
                     Eval_G_CB eval_g,
                     Eval_Grad_F_CB eval_grad_f,
                     Eval_Jac_G_CB eval_jac_g,
                     Eval_H_CB eval_h,
                     ScalingParams_CB scaling,
                     Intermediate_CB* intermediate_cb);

    /** Getters for the solution data */

    /**
     * Get optimal variables and multipliers. This function returns the arguments of the
     * optimization.
     */
    SolverData get_solution_arguments();

    Number get_objective_value();

    Number* get_constraint_function_values();

    /** Default destructor */
    virtual ~StdInterfaceTNLP();
    //@}

    /// Allow the user to set the user data pointer after the problem has already been created.
    /// This must be set before calling any of the user specified callbacks.
    void set_user_data(UserDataPtr user_data);

    void init_solution();

    void preallocate_solution_data(Index n, Index m);

    /**@name methods to gather information about the NLP. These methods are
     * overloaded from TNLP. See TNLP for their more detailed documentation. */
    //@{
    /** returns dimensions of the nlp. Overloaded from TNLP */
    virtual bool get_nlp_info(Index& n, Index& m, Index& nnz_jac_g,
                              Index& nnz_h_lag, IndexStyleEnum& index_style);

    /** returns bounds of the nlp. Overloaded from TNLP */
    virtual bool get_bounds_info(Index n, Number* x_l, Number* x_u,
                                 Index m, Number* g_l, Number* g_u);

    /** returns scaling parameters (if nlp_scaling_method is selected
     * as user-scaling). Overloaded from TNLP */
    virtual bool get_scaling_parameters(Number& obj_scaling,
                                        bool& use_x_scaling, Index n,
                                        Number* x_scaling,
                                        bool& use_g_scaling, Index m,
                                        Number* g_scaling);

    /** provides a starting point for the nlp variables. Overloaded from TNLP */
    virtual bool get_starting_point(Index n, bool init_x, Number* x,
                                    bool init_z, Number* z_L, Number* z_U,
                                    Index m, bool init_lambda, Number* lambda);

    /** evaluates the objective value for the nlp. Overloaded from TNLP */
    virtual bool eval_f(Index n, const Number* x, bool new_x,
                        Number& obj_value);

    /** evaluates the gradient of the objective for the
     *  nlp. Overloaded from TNLP */
    virtual bool eval_grad_f(Index n, const Number* x, bool new_x,
                             Number* grad_f);

    /** evaluates the constraint residuals for the nlp. Overloaded from TNLP */
    virtual bool eval_g(Index n, const Number* x, bool new_x, Index m,
                        Number* g);

    /** specifies the jacobian structure (if values is NULL) and
     *  evaluates the jacobian values (if values is not NULL) for the
     *  nlp. Overloaded from TNLP */
    virtual bool eval_jac_g(Index n, const Number* x, bool new_x, Index m,
                            Index nele_jac, Index* iRow, Index *jCol,
                            Number* values);

    /** specifies the structure of the hessian of the lagrangian (if values is NULL) and
     *  evaluates the values (if values is not NULL). Overloaded from TNLP */
    virtual bool eval_h(Index n, const Number* x, bool new_x,
                        Number obj_factor, Index m, const Number* lambda,
                        bool new_lambda, Index nele_hess, Index* iRow,
                        Index* jCol, Number* values);

    /** Intermediate Callback method for the user.  Overloaded from TNLP */
    virtual bool intermediate_callback(AlgorithmMode mode,
                                       Index iter, Number obj_value,
                                       Number inf_pr, Number inf_du,
                                       Number mu, Number d_norm,
                                       Number regularization_size,
                                       Number alpha_du, Number alpha_pr,
                                       Index ls_trials,
                                       const IpoptData* ip_data,
                                       IpoptCalculatedQuantities* ip_cq);
    //@}

    /** @name Solution Methods */
    //@{
    virtual void finalize_solution(SolverReturn status,
                                   Index n, const Number* x, const Number* z_L, const Number* z_U,
                                   Index m, const Number* g, const Number* lambda,
                                   Number obj_value,
                                   const IpoptData* ip_data,
                                   IpoptCalculatedQuantities* ip_cq);
    //@}

  private:
    /** Journlist */
    SmartPtr<const Journalist> jnlst_;

    /** @name Information about the problem */
    //@{
    /** Starting value of the iRow and jCol parameters for matrices */
    const Index index_style_;
    /** Pointer to callback function setting array size information. */
    Sizes_CB sizes_;
    /** Pointer to callback function initializing the iterates. */
    Init_CB init_;
    /** Pointer to callback function evaluating lower and upper bounds on
     *  variables and constraint functions */
    Bounds_CB bounds_;
    /** Pointer to callback function evaluating value of objective function */
    Eval_F_CB eval_f_;
    /**  Pointer to callback function evaluating value of constraints */
    Eval_G_CB eval_g_;
    /** Pointer to callback function evaluating gradient of objective
     *  function */
    Eval_Grad_F_CB eval_grad_f_;
    /** Pointer to callback function evaluating Jacobian of constraints */
    Eval_Jac_G_CB eval_jac_g_;
    /** Pointer to callback function evaluating Hessian of Lagrangian */
    Eval_H_CB eval_h_;
    /** Pointer to callback function for setting scaling parameters */
    ScalingParams_CB scaling_;
    /** Pointer to intermediate callback function giving control to user */
    Intermediate_CB* intermediate_cb_;
    /** Pointer to user data */
    UserDataPtr user_data_;
    //@}

    /** Solution data */
    std::vector<Number> x_sol_;
    std::vector<Number> z_L_sol_;
    std::vector<Number> z_U_sol_;
    std::vector<Number> g_sol_;
    std::vector<Number> lambda_sol_;
    Number obj_sol_;

    /** Overloaded Equals Operator */
    void operator=(const StdInterfaceTNLP&);
    //@}

    /** Deleted Default Constructor */
    StdInterfaceTNLP();

    /** Deleted Copy Constructor */
    StdInterfaceTNLP(const StdInterfaceTNLP&);
  };

} // namespace Ipopt

#endif
