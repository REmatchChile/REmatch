rm -r autogenerated/*
java -Xmx500M -cp "/usr/local/lib/antlr-4.11.1-complete.jar" org.antlr.v4.Tool -Dlanguage=Cpp -o autogenerated -no-listener -visitor REmatchLexer.g4 
java -Xmx500M -cp "/usr/local/lib/antlr-4.11.1-complete.jar" org.antlr.v4.Tool -Dlanguage=Cpp -o autogenerated -no-listener -visitor REmatchParser.g4