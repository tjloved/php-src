--TEST--
Temporary array expressions can be iterated by reference
--FILE--
<?php

function fn513276()
{
    $a = 'a';
    $b = 'b';
    foreach ([&$a, &$b] as &$value) {
        $value .= '-foo';
    }
    var_dump($a, $b);
}
fn513276();
--EXPECT--
string(5) "a-foo"
string(5) "b-foo"
