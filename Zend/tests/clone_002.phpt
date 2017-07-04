--TEST--
Testing multiple clone statements
--FILE--
<?php

class foo
{
}
function fn1997692152()
{
    $a = clone clone $b = new stdClass();
    var_dump($a == $b);
    $c = clone clone clone $b = new stdClass();
    var_dump($a == $b, $b == $c);
    $d = clone $a = $b = new foo();
    var_dump($a == $d, $b == $d, $c == $a);
}
fn1997692152();
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(false)
