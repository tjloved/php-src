--TEST--
Smart branches 1
--FILE--
<?php

function test(int $a, int $b) {
    $result = $a < $b;
    $x = 1;
    if ($result) {
        return $result;
    }
}

var_dump(test(1, 2));

?>
--EXPECT--
bool(true)
