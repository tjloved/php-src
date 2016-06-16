--TEST--
Bug #64354 (Unserialize array of objects whose class can't be autoloaded fail)
--FILE--
<?php

class B implements Serializable
{
    public function serialize()
    {
        throw new Exception("serialize");
        return NULL;
    }
    public function unserialize($data)
    {
    }
}
function fn770612995()
{
    $data = array(new B());
    try {
        serialize($data);
    } catch (Exception $e) {
        var_dump($e->getMessage());
    }
}
fn770612995();
--EXPECTF--
string(9) "serialize"
