--TEST--
GC 028: GC and destructors
--INI--
zend.enable_gc=1
--FILE--
<?php

class Foo
{
    public $bar;
    function __destruct()
    {
        if ($this->bar !== null) {
            unset($this->bar);
        }
    }
}
class Bar
{
    public $foo;
    function __destruct()
    {
        if ($this->foo !== null) {
            unset($this->foo);
        }
    }
}
function fn513665184()
{
    $foo = new Foo();
    $bar = new Bar();
    $foo->bar = $bar;
    $bar->foo = $foo;
    unset($foo);
    unset($bar);
    var_dump(gc_collect_cycles());
}
fn513665184();
--EXPECT--
int(2)
