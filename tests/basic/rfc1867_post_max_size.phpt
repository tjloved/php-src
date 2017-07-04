--TEST--
rfc1867 post_max_size
--INI--
post_max_size=1
--POST_RAW--
Content-Type: multipart/form-data; boundary=---------------------------20896060251896012921717172737
-----------------------------20896060251896012921717172737
Content-Disposition: form-data; name="foobar"

1
-----------------------------20896060251896012921717172737--
--FILE--
<?php

function fn1152570500()
{
    var_dump($_FILES);
    var_dump($_POST);
}
fn1152570500();
--EXPECTF--
Warning: POST Content-Length of %d bytes exceeds the limit of 1 bytes in %s
array(0) {
}
array(0) {
}
