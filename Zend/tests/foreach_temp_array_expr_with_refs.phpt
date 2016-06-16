--TEST--
Temporary array expressions can be iterated by reference
--FILE--
<?php

function fn922502175()
{
    $a = 'a';
    $b = 'b';
    foreach ([&$a, &$b] as &$value) {
        $value .= '-foo';
    }
    var_dump($a, $b);
}
fn922502175();
--EXPECT--
string(5) "a-foo"
string(5) "b-foo"
