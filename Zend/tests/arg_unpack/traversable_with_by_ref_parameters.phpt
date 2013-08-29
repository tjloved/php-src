--TEST--
Traversables cannot be unpacked into by-reference parameters
--FILE--
<?php

function test($val1, $val2, $val3, &$ref) {
    $ref = 42;
}

function gen($array) {
    foreach ($array as $element) {
        yield $element;
    }
}

test(...gen([1, 2, 3]), $a);
var_dump($a);
test(1, 2, 3, $b, ...gen([4, 5, 6]));
var_dump($b);

try {
    test(...gen([1, 2, 3, 4]));
} catch (Exception $e) { var_dump($e->getMessage()); }

try {
    test(1, 2, ...gen([3, 4]));
} catch (Exception $e) { var_dump($e->getMessage()); }

try {
    test(...gen([1, 2]), ...gen([3, 4]));
} catch (Exception $e) { var_dump($e->getMessage()); }

?>
--EXPECT--
int(42)
int(42)
string(72) "Cannot pass by-reference argument 4 of test() by unpacking a Traversable"
string(72) "Cannot pass by-reference argument 4 of test() by unpacking a Traversable"
string(72) "Cannot pass by-reference argument 4 of test() by unpacking a Traversable"
