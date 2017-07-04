--TEST--
Bug #70397 (Segmentation fault when using Closure::call and yield)
--FILE--
<?php

function fn2056704466()
{
    $f = function () {
        $this->value = true;
        (yield $this->value);
    };
    var_dump($f->call(new class
    {
    })->current());
}
fn2056704466();
--EXPECT--
bool(true)
