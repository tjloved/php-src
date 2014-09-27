--TEST--
eval() test
--FILE--
<?php 

eval("function test() { echo \"hey, this is a function inside an eval()!\\n\"; }");

$i=0;
while ($i<10) {
  eval("echo \"hey, this is a regular echo'd eval()\\n\";");
  test();
  $i++;
}

eval('-');
--EXPECTF--
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!
hey, this is a regular echo'd eval()
hey, this is a function inside an eval()!

Fatal error: Uncaught exception 'ParseException' with message 'syntax error, unexpected end of file' in %s(12) : eval()'d code:1
Stack trace:
#0 {main}
  thrown in %s(12) : eval()'d code on line 1
