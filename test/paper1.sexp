(CompilationUnit
 (TypeDeclaration
  "Modifier public"
  "SimpleName Test"
  (MethodDeclaration
   "Modifier public"
   ("SimpleType String"
    "SimpleName String")
   "SimpleName foo"
   (SingleVariableDeclaration
    "PrimitiveType int"
    "SimpleName i")
   (Block
    (IfStatement
     ("InfixExpression =="
      "SimpleName i"
      "NumberLiteral 0")
     (ReturnStatement "StringLiteral Foo!"))))))
