--TEST--
Bug #68262: Broken reference across cloned objects
--FILE--
<?php

class C
{
    public $p;
}
function fn505325167()
{
    $first = new C();
    $first->p = 'init';
    $clone = clone $first;
    $ref =& $first->p;
    unset($ref);
    $clone = clone $first;
    $clone->p = 'foo';
    var_dump($first->p);
}
fn505325167();
--EXPECT--
string(4) "init"
