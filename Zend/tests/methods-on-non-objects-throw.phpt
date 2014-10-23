--TEST--
Convert errors to exceptions from method calls on non-objects raise recoverable errors
--FILE--
<?php

$x= null;
echo "Calling...\n";
try {
  $x->method();
} catch (EngineException $e) {
  echo "Caught ", $e->getMessage(), "!\n";
}
echo "Alive\n";
?>
--EXPECTF--
Calling...
Caught Call to a member function method() on null!
Alive
