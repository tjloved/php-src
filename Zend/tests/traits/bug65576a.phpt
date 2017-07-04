--TEST--
Bug #65576 (Constructor from trait conflicts with inherited constructor)
--FILE--
<?php

trait T
{
    public function __construct()
    {
        echo "Trait contructor\n";
    }
}
class A
{
    public function __construct()
    {
        echo "Parent constructor\n";
    }
}
class B extends A
{
    use T;
}
function fn1824569972()
{
    new B();
}
fn1824569972();
--EXPECT--
Trait contructor

