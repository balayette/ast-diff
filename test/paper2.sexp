(CompilationUnit
 (TypeDeclaration
  "Modifier public"
  "SimpleName Test"
  (MethodDeclaration
   "Modifier private"
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
     (ReturnStatement "StringLiteral Bar")
     (IfStatement
      ("InfixExpression =="
       "SimpleName i"
       ("PrefixExpression -"
	"NumberLiteral 1"))
      (ReturnStatement "StringLiteral Foo!")))))))
