--TEST--
Smart branches 2
--FILE--
<?php

function test(int $a, int $b, bool $return) {
    $result = $a < $b;
    if ($return) {
        return $result;
    }
}

var_dump(test(1, 2, false));

?>
--EXPECT--
NULL
