--TEST--
Basic class support - attempting to create a reference to a class constant
--FILE--
<?php
  class aclass
  {
      const myConst = "hello";
  }
  
  echo "\nAttempting to create a reference to a class constant - should be parse error.\n";
  $a = &aclass::myConst;
?>
--EXPECTF--
Fatal error: Uncaught exception 'ParseException' with message '%s' in %s:%d
Stack trace:
#0 {main}
  thrown in %s on line %d
