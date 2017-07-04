--TEST--
Testing __debugInfo() magic method with bad returns STRING
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
function fn2096564407()
{
    $c = new C("foo");
    var_dump($c);
}
fn2096564407();
--EXPECTF--
Fatal error: __debuginfo() must return an array in %s%eZend%etests%edebug_info-error-str.php on line %d
