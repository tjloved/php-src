--TEST--
Bug #24499 (bogus handling of a public property as a private one)
--FILE--
<?php

class Id
{
    private $id = "priv";
    public function tester($obj)
    {
        $obj->id = "bar";
    }
}
function fn1258483975()
{
    $id = new Id();
    @($obj->foo = "bar");
    $id->tester($obj);
    print_r($obj);
}
fn1258483975();
--EXPECT--
stdClass Object
(
    [foo] => bar
    [id] => bar
)
