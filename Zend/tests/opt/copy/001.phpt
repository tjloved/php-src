--TEST--
Copy 001
--FILE--
<?php

function test() {
    $b = $a;
    var_dump($b);
    var_dump($b);
}

test();

?>
--EXPECTF--
Notice: Undefined variable: a in %s on line %d
NULL
NULL
