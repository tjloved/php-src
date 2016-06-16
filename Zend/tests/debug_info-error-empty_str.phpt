--TEST--
Testing __debugInfo() magic method with bad returns EMPTY STRING
--FILE--
<?php

class C
{
    public $val;
    public function __debugInfo()
    {
        return $this->val;
    }
    public function __construct($val)
    {
        $this->val = $val;
    }
}
function fn1164292116()
{
    $c = new C("");
    var_dump($c);
}
fn1164292116();
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-empty_str.php on line %d
