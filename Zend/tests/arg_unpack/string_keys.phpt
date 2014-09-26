--TEST--
Argument unpacking does not work with string keys (forward compatibility for named args)
--FILE--
<?php

try {
    var_dump(...[1, 2, "foo" => 3, 4]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    var_dump(...new ArrayIterator([1, 2, "foo" => 3, 4]));
} catch (Exception $e) {
    var_dump($e->getMessage());
}

?>
--EXPECTF--
string(36) "Cannot unpack array with string keys"
string(42) "Cannot unpack Traversable with string keys"
