--TEST--
Constant propagation 002
--FILE--
<?php

function test($b) {
    $a = 1;
    if ($b) {
        $a = 2;
    } else {
        $a = 2;
    }
    return $a; // 2
}

var_dump(test(true));
var_dump(test(false));

?>
--EXPECT--
int(2)
int(2)
