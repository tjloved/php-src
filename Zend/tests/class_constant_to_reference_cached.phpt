--TEST--
Conversion of a class constant to a reference after it has been cached
--FILE--
<?php

class Test
{
    const TEST = 'TEST';
    private $prop;
    public function readConst()
    {
        $this->prop = self::TEST;
    }
}
function doTest()
{
    $obj = new Test();
    $obj->readConst();
    unset($obj);
    var_dump(Test::TEST);
}
function fn1919974483()
{
    doTest();
    eval('class Test2 extends Test {}');
    doTest();
}
fn1919974483();
--EXPECT--
string(4) "TEST"
string(4) "TEST"
