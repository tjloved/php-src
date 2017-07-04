--TEST--
Bug #50174 (Incorrectly matched docComment)
--SKIPIF--
<?php if (!extension_loaded('reflection') || !extension_loaded('spl')) print "skip SPL and reflection extensions required"; ?>
--FILE--
<?php

class TestClass
{
    /** const comment */
    const C = 0;
    function x()
    {
    }
}
class TestClass2
{
    /** const comment */
    const C = 0;
    public $x;
}
function fn1001001188()
{
    $rm = new ReflectionMethod('TestClass', 'x');
    var_dump($rm->getDocComment());
    $rp = new ReflectionProperty('TestClass2', 'x');
    var_dump($rp->getDocComment());
}
fn1001001188();
--EXPECT--
bool(false)
bool(false)
