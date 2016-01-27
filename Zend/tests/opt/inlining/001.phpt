--TEST--
Inlining 001
--FILE--
<?php

function test2($a) {
    $a .= "a";
}

function test(string $a) {
    for ($i = 0; $i < 2; $i++) {
        test2($a);
    }
}

test("foo");

?>
===DONE===
--EXPECT--
===DONE===
