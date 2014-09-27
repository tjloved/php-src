--TEST--
Basic class support - attempting to modify a class constant by assignment
--FILE--
<?php
  class aclass
  {
      const myConst = "hello";
  }
  
  echo "\nTrying to modify a class constant directly - should be parse error.\n";
  aclass::myConst = "no!!";
  var_dump(aclass::myConst);
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s'' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
