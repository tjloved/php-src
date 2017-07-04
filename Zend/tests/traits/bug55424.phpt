--TEST--
Bug #55424 (Method got missing from class when a trait defined an abstract method to express a requirement)
--FILE--
<?php

trait ATrait
{
    function setRequired()
    {
        $this->setAttribute();
    }
    abstract function setAttribute();
}
class Base
{
    function setAttribute()
    {
    }
}
class MyClass extends Base
{
    use ATrait;
}
function fn1096451463()
{
    $i = new Base();
    $i->setAttribute();
    $t = new MyClass();
    /* setAttribute used to disappear for no good reason. */
    $t->setRequired();
    echo 'DONE';
}
fn1096451463();
--EXPECT--
DONE
