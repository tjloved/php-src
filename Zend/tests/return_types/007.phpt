--TEST--
Return value is subclass of return type

--FILE--
<?php

class foo
{
}
class qux extends foo
{
    public function foo() : foo
    {
        return $this;
    }
}
function fn1604886750()
{
    $qux = new qux();
    var_dump($qux->foo());
}
fn1604886750();
--EXPECTF--
object(qux)#%d (%d) {
}
