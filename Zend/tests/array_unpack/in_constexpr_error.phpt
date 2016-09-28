--TEST--
Array unpacking is not supported in constant expressions
--FILE--
<?php

const FOO = [...[]];

?>
--EXPECTF--
Fatal error: Constant expression contains invalid operations in %s on line %d
