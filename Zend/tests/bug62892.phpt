--TEST--
Bug #62892 (ReflectionClass::getTraitAliases crashes on importing trait methods as private)
--FILE--
<?php

trait myTrait
{
    public function run()
    {
    }
}
class myClass
{
    use myTrait {
        MyTrait::run as private;
    }
}
function fn766656158()
{
    $class = new \ReflectionClass('myClass');
    var_dump($class->getTraitAliases());
}
fn766656158();
--EXPECTF--
array(0) {
}
