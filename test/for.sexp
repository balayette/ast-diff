(Module for.sexp
 (For
  (Declaration int i
   (Integer 0))
  (BinaryOp Greater 
   (VariableReference i)
   (Integer 10))
  (CompoundStatement
   (FunctionCall (FunctionRef printf int variadic)
    (StringLiteral "%d\n")
    (VariableReference i))
   (UnaryOp Postcrement i))))
