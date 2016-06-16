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
function fn221063456()
{
    (new Test())->method();
}
fn221063456();
--EXPECT--
object(Test)#1 (0) {
}
