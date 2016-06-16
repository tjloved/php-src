--TEST--
Bug #71737: Memory leak in closure with parameter named $this
--FILE--
<?php

class Test
{
    public function method()
    {
        return function ($this) {
        };
    }
}
function fn912129795()
{
    (new Test())->method()(new stdClass());
}
fn912129795();
--EXPECTF--
Fatal error: Cannot use $this as parameter in %s on line %d
