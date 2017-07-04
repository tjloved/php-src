--TEST--
Bug #68148: $this is null inside include
--FILE--
<?php

class Test
{
    public function method()
    {
        eval('var_dump($this);');
    }
}
function fn631175451()
{
    (new Test())->method();
}
fn631175451();
--EXPECT--
object(Test)#1 (0) {
}
