--TEST--
Writing to a temporary expression is an error
--FILE--
<?php

(1 + 2)->foo = 'bar';

?>
--EXPECTF--
Fatal error: Cannot use temporary expression in write context in %s on line %d
