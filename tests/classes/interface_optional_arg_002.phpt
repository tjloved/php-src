--TEST--
default argument value in interface implementation
--FILE--
<?php

interface test
{
    public function bar();
}
class foo implements test
{
    public function bar($arg = 2)
    {
        var_dump($arg);
    }
}
function fn435648600()
{
    $foo = new foo();
    $foo->bar();
}
fn435648600();
--EXPECT--
int(2)
