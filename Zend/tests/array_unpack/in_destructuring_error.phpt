--TEST--
Array unpacking is not supported in destructuring assignments
--FILE--
<?php

[...$foo] = [];

?>
--EXPECTF--
Fatal error: Cannot use unpacking in [] and list() assignments in %s on line %d
