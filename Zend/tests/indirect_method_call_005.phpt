--TEST--
Testing array dereferencing from instance with ArrayObject
--FILE--
<?php

class foo extends ArrayObject
{
    public function __construct($arr)
    {
        parent::__construct($arr);
    }
}
function fn1696318599()
{
    var_dump((new foo(array(1, array(4, 5), 3)))[1][0]);
    // int(4)
}
fn1696318599();
--EXPECT--
int(4)
