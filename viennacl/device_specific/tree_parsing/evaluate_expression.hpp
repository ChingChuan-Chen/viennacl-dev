#ifndef VIENNACL_DEVICE_SPECIFIC_TREE_PARSING_ELEMENTWISE_EXPRESSION_HPP
#define VIENNACL_DEVICE_SPECIFIC_TREE_PARSING_ELEMENTWISE_EXPRESSION_HPP

/* =========================================================================
   Copyright (c) 2010-2013, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.
   Portions of this software are copyright by UChicago Argonne, LLC.

                            -----------------
                  ViennaCL - The Vienna Computing Library
                            -----------------

   Project Head:    Karl Rupp                   rupp@iue.tuwien.ac.at

   (A list of authors and contributors can be found in the PDF manual)

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */


/** @file viennacl/generator/helpers.hpp
    @brief several code generation helpers
*/

#include <set>

#include "CL/cl.h"

#include "viennacl/forwards.h"

#include "viennacl/scheduler/forwards.h"

#include "viennacl/device_specific/forwards.h"
#include "viennacl/device_specific/tree_parsing/traverse.hpp"

namespace viennacl{

  namespace device_specific{

    namespace tree_parsing{

      /** @brief generate a string from an operation_node_type */
      inline const char * evaluate(scheduler::operation_node_type type){
        // unary expression
        switch(type){
          //Function
          case scheduler::OPERATION_UNARY_ABS_TYPE : return "abs";
          case scheduler::OPERATION_UNARY_ACOS_TYPE : return "acos";
          case scheduler::OPERATION_UNARY_ASIN_TYPE : return "asin";
          case scheduler::OPERATION_UNARY_ATAN_TYPE : return "atan";
          case scheduler::OPERATION_UNARY_CEIL_TYPE : return "ceil";
          case scheduler::OPERATION_UNARY_COS_TYPE : return "cos";
          case scheduler::OPERATION_UNARY_COSH_TYPE : return "cosh";
          case scheduler::OPERATION_UNARY_EXP_TYPE : return "exp";
          case scheduler::OPERATION_UNARY_FABS_TYPE : return "fabs";
          case scheduler::OPERATION_UNARY_FLOOR_TYPE : return "floor";
          case scheduler::OPERATION_UNARY_LOG_TYPE : return "log";
          case scheduler::OPERATION_UNARY_LOG10_TYPE : return "log10";
          case scheduler::OPERATION_UNARY_SIN_TYPE : return "sin";
          case scheduler::OPERATION_UNARY_SINH_TYPE : return "sinh";
          case scheduler::OPERATION_UNARY_SQRT_TYPE : return "sqrt";
          case scheduler::OPERATION_UNARY_TAN_TYPE : return "tan";
          case scheduler::OPERATION_UNARY_TANH_TYPE : return "tanh";

          case scheduler::OPERATION_BINARY_ELEMENT_ARGMAX_TYPE : return "argmax";
          case scheduler::OPERATION_BINARY_ELEMENT_ARGMIN_TYPE : return "argmin";
          case scheduler::OPERATION_BINARY_ELEMENT_POW_TYPE : return "pow";

          //Arithmetic
          case scheduler::OPERATION_UNARY_MINUS_TYPE : return "-";
          case scheduler::OPERATION_BINARY_ASSIGN_TYPE : return "=";
          case scheduler::OPERATION_BINARY_INPLACE_ADD_TYPE : return "+=";
          case scheduler::OPERATION_BINARY_INPLACE_SUB_TYPE : return "-=";
          case scheduler::OPERATION_BINARY_ADD_TYPE : return "+";
          case scheduler::OPERATION_BINARY_SUB_TYPE : return "-";
          case scheduler::OPERATION_BINARY_MULT_TYPE : return "*";
          case scheduler::OPERATION_BINARY_ELEMENT_PROD_TYPE : return "*";
          case scheduler::OPERATION_BINARY_DIV_TYPE : return "/";
          case scheduler::OPERATION_BINARY_ELEMENT_DIV_TYPE : return "/";
          case scheduler::OPERATION_BINARY_ACCESS_TYPE : return "[]";

          //Relational
          case scheduler::OPERATION_BINARY_ELEMENT_EQ_TYPE : return "isequal";
          case scheduler::OPERATION_BINARY_ELEMENT_NEQ_TYPE : return "isnotequal";
          case scheduler::OPERATION_BINARY_ELEMENT_GREATER_TYPE : return "isgreater";
          case scheduler::OPERATION_BINARY_ELEMENT_GEQ_TYPE : return "isgreaterequal";
          case scheduler::OPERATION_BINARY_ELEMENT_LESS_TYPE : return "isless";
          case scheduler::OPERATION_BINARY_ELEMENT_LEQ_TYPE : return "islessequal";

          case scheduler::OPERATION_BINARY_ELEMENT_FMAX_TYPE : return "fmax";
          case scheduler::OPERATION_BINARY_ELEMENT_FMIN_TYPE : return "fmin";

          //Unary
          case scheduler::OPERATION_UNARY_TRANS_TYPE : return "trans";

          //Binary
          case scheduler::OPERATION_BINARY_INNER_PROD_TYPE : return "iprod";
          case scheduler::OPERATION_BINARY_MAT_MAT_PROD_TYPE : return "mmprod";
          case scheduler::OPERATION_BINARY_MAT_VEC_PROD_TYPE : return "mvprod";
          case scheduler::OPERATION_BINARY_VECTOR_DIAG_TYPE : return "diag";
          case scheduler::OPERATION_BINARY_MATRIX_DIAG_TYPE : return "diag";
          case scheduler::OPERATION_BINARY_MATRIX_ROW_TYPE : return "row";
          case scheduler::OPERATION_BINARY_MATRIX_COLUMN_TYPE : return "col";

          default : throw generator_not_supported_exception("Unsupported operator");
        }
      }

      inline const char * evaluate_str(scheduler::operation_node_type type){
        switch(type){
        case scheduler::OPERATION_UNARY_MINUS_TYPE : return "mi";
        case scheduler::OPERATION_BINARY_ASSIGN_TYPE : return "as";
        case scheduler::OPERATION_BINARY_INPLACE_ADD_TYPE : return "iad";
        case scheduler::OPERATION_BINARY_INPLACE_SUB_TYPE : return "isu";
        case scheduler::OPERATION_BINARY_ADD_TYPE : return "ad";
        case scheduler::OPERATION_BINARY_SUB_TYPE : return "su";
        case scheduler::OPERATION_BINARY_MULT_TYPE : return "mu";
        case scheduler::OPERATION_BINARY_ELEMENT_PROD_TYPE : return "epr";
        case scheduler::OPERATION_BINARY_DIV_TYPE : return "di";
        case scheduler::OPERATION_BINARY_ELEMENT_DIV_TYPE : return "edi";
        case scheduler::OPERATION_BINARY_ACCESS_TYPE : return "ac";
          default : return evaluate(type);
        }
      }


      /** @brief functor for generating the expression string from a statement */
      class evaluate_expression_traversal: public traversal_functor{
        private:
          index_tuple index_;
          int simd_element_;
          std::string & str_;
          mapping_type const & mapping_;

        public:
          evaluate_expression_traversal(index_tuple const & index, int simd_element, std::string & str, mapping_type const & mapping) : index_(index), simd_element_(simd_element), str_(str), mapping_(mapping){ }

          void call_before_expansion(scheduler::statement const & statement, unsigned int root_idx) const
          {
              scheduler::statement_node const & root_node = statement.array()[root_idx];
              if((root_node.op.type_family==scheduler::OPERATION_UNARY_TYPE_FAMILY || utils::elementwise_function(root_node.op))
                  && !utils::node_leaf(root_node.op))
                  str_+=evaluate(root_node.op.type);
              str_+="(";

          }
          void call_after_expansion(scheduler::statement const & statement, unsigned int root_idx) const
          {
            str_+=")";
          }

          void operator()(scheduler::statement const & statement, unsigned int root_idx, node_type leaf) const
          {
            scheduler::statement_node const & root_node = statement.array()[root_idx];
            mapping_type::key_type key = std::make_pair(root_idx, leaf);
            if(leaf==PARENT_NODE_TYPE)
            {
              if(utils::node_leaf(root_node.op))
                str_ += mapping_.at(key)->evaluate(index_, simd_element_);
              else if(utils::elementwise_operator(root_node.op))
                str_ += evaluate(root_node.op.type);
              else if(root_node.op.type_family!=scheduler::OPERATION_UNARY_TYPE_FAMILY && utils::elementwise_function(root_node.op))
                str_ += ",";
            }
            else
            {
              if(leaf==LHS_NODE_TYPE)
              {
                if(root_node.lhs.type_family!=scheduler::COMPOSITE_OPERATION_FAMILY)
                  str_ += mapping_.at(key)->evaluate(index_,simd_element_);
              }

              if(leaf==RHS_NODE_TYPE)
              {
                if(root_node.rhs.type_family!=scheduler::COMPOSITE_OPERATION_FAMILY)
                  str_ += mapping_.at(key)->evaluate(index_,simd_element_);
              }
            }
          }
      };

      inline std::string evaluate_expression(scheduler::statement const & statement, unsigned int root_idx, index_tuple const & index,
                                             int simd_element, mapping_type const & mapping, node_type leaf)
      {
        std::string res;
        evaluate_expression_traversal traversal_functor(index, simd_element, res, mapping);
        scheduler::statement_node const & root_node = statement.array()[root_idx];

        if(leaf==RHS_NODE_TYPE)
        {
          if(root_node.rhs.type_family==scheduler::COMPOSITE_OPERATION_FAMILY)
            traverse(statement, root_node.rhs.node_index, traversal_functor, false);
          else
            traversal_functor(statement, root_idx, leaf);
        }
        else if(leaf==LHS_NODE_TYPE)
        {
          if(root_node.lhs.type_family==scheduler::COMPOSITE_OPERATION_FAMILY)
            traverse(statement, root_node.lhs.node_index, traversal_functor, false);
          else
            traversal_functor(statement, root_idx, leaf);
        }
        else
          traverse(statement, root_idx, traversal_functor, false);

        return res;
      }




    }
  }
}
#endif
