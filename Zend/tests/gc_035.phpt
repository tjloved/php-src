--TEST--
GC 035: Lost inner-cycles garbage
--FILE--
<?php

class A
{
    public $a;
    public $x;
    function __destruct()
    {
        unset($this->x);
    }
}
function fn319280655()
{
    $a = new A();
    $a->a = $a;
    $a->x = [];
    $a->x[] =& $a->x;
    $a->x[] = $a;
    var_dump(gc_collect_cycles());
    unset($a);
    var_dump(gc_collect_cycles());
    var_dump(gc_collect_cycles());
}
fn319280655();
--EXPECT--
int(0)
int(2)
int(0)
