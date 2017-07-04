--TEST--
rfc1867 missing boundary
--INI--
post_max_size=1024
--POST_RAW--
Content-Type: multipart/form-data
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="foobar"

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php

function fn1124529765()
{
    var_dump($_FILES);
    var_dump($_POST);
}
fn1124529765();
--EXPECTF--
Warning: Missing boundary in multipart/form-data POST data in %s
array(0) {
}
array(0) {
}
