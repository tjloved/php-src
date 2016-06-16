--TEST--
Bug #70397 (Segmentation fault when using Closure::call and yield)
--FILE--
<?php

function fn349600239()
{
    $f = function () {
        $this->value = true;
        (yield $this->value);
    };
    var_dump($f->call(new class
    {
    })->current());
}
fn349600239();
--EXPECT--
bool(true)
