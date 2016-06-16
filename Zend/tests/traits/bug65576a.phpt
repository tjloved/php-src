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
function fn130760103()
{
    new B();
}
fn130760103();
--EXPECT--
Trait contructor

