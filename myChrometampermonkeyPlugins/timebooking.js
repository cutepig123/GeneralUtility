// ==UserScript==
// @name         TimeBooking
// @namespace    http://tampermonkey.net/
// @version      0.1
// @description  try to take over the world!
// @author       You
// @match        https://www.citibank.com.hk/*
// @match        https://www.services.online-banking.hsbc.com.hk/*
// @match        https://e-banking1.hangseng.com/*
// @grant          GM_xmlhttpRequest
// ==/UserScript==

(function() {
    'use strict';

    //http://atsnts82/stagegate/TimeBooking.aspx?Year=2016&WeekOfYear=16

    function updateTextArea(){
        var xpath='//textarea';
        var tags_data_image =document.evaluate(xpath, document, null, XPathResult.ANY_TYPE,null);

        var textareavalue='';
        var tags=[];
        for(var tag=tags_data_image.iterateNext(); tag; tag=tags_data_image.iterateNext())
        {
            tags.push(tag);
            if(tag.value.length>0 && textareavalue.length===0) {
                textareavalue = tag.value;

            }
        }

        if(textareavalue.length===0)
            alert('No textarea has data!');

        for(var i=0; i<tags.length; i++)
        {
            tag =tags[i];
            tag.value = textareavalue;
        }
    }

     function updateSelect(){
        var xpath='//select[contains(@name,"gvddlManDay")]';
        var tags_data_image =document.evaluate(xpath, document, null, XPathResult.ANY_TYPE,null);

        var tags=[];
        for(var tag=tags_data_image.iterateNext(); tag; tag=tags_data_image.iterateNext())
        {
            tags.push(tag);
        }

        xpath='//select[contains(@name,"gvddlProjectNo")]';
        tags_data_image =document.evaluate(xpath, document, null, XPathResult.ANY_TYPE,null);
        for(tag=tags_data_image.iterateNext(); tag; tag=tags_data_image.iterateNext())
        {
            tags.push(tag);
        }


         for(var i=0; i<tags.length; i++)
        {
            tag =tags[i];
            //tag.selectedIndex = 0;//Not work!
            tag.value = tag.options[1].value;
        }
    }

     function getInfo(xpath){
        var tags_data_image =document.evaluate(xpath, document, null, XPathResult.ANY_TYPE,null);

        var textareavalue='';
        var tags=[];
        for(var tag=tags_data_image.iterateNext(); tag; tag=tags_data_image.iterateNext())
        {
            console.log(">> " + tag.innerText);
            tags.push(tag.innerText);
        }

        return tags;
    }

    function updateAll(){



        var currentLocation = window.location;
        var title, balance;

        if(currentLocation.host.indexOf('citibank')>=0){
            title = getInfo("//a[@id='cmlink_AccountNameLink']");
            balance = getInfo("//div[@class='cT-valueItem']/span[@class='cT-balanceIndicator1 ']");
        }
        else if(currentLocation.host.indexOf('hsbc')>=0){
            title=['khd', 'usd', 'invest', 'hkd292', 'credit'];
            var balances_xpath=["//span[@id='l5pIclzHeguKJh2mPS3yjCCav19PYeaKZddPwiPQNFA=||HKD||2A833HKD       HKDPVSAV       ||HK']",
                           "//span[@id='l5pIclzHeguKJh2mPS3yjCCav19PYeaKZddPwiPQNFA=||USD||2F833USD       USDPVSAV       ||HK']",
                           "//span[@id='l5pIclzHeguKJh2mPS3yjCCav19PYeaKZddPwiPQNFA=||HKD||2R833HKD       HKDPVINV       ||HK']",
                           "//span[@class='fr']/span[@id='l5pIclzHeguKJh2mPS3yjIlO2mrWj4_add_vuWd8eH61wjw=||HKD||2C292HKD       HKDSSSSA       ||HK']",
                           "//span[@id='l5pIclzHeguKJh2mPS3yjL97YmPyxdxUXqsED7ZtWP34iNVKyQjyz8sR7tX9V6Lm||HKD||3PA  HKD       HKDCRAVR       ||HK']"];
            balance = [];

            for(var i=0; i<balances_xpath.length; i++){
                var t=getInfo(balances_xpath[i]);
                if(t.length!=1){
                    alert('t.length not good' + t.toString());
                }
                balance.push(t[0]);
            }
        }
		else if(currentLocation.host.indexOf('hangseng')>=0){
            title=['hkd1', 'hkd2', 'credit', 'loan'];
            balances_xpath=["//table[@id='accSumTable']/tbody/tr[@class='lvl2']/td[@class='right']",
                           "//table[@id='accSumTable']/tbody/tr[@class='lvl2 last']/td[@class='right']",
                           "//table[@id='accSumTable']/tbody/tr[@class='greenRow']/td[@class='right']",
                           "//table[@id='loanSumTable']/tbody/tr[2]/td[@class='right']"];
            balance = [];

            for(i=0; i<balances_xpath.length; i++){
                t=getInfo(balances_xpath[i]);
                if(t.length!=1){
                    alert('t.length not good' + t.toString());
                }
                balance.push(t[0]);
            }
        }
        else{
            alert('Invalid url: ' + currentLocation);
        }

        if(title.length!=balance.length)
        {
            alert('logic bug!');return;
        }

        var s=''
        for(i=0; i<title.length;i++){
            console.log(">> " + title[i] + "\t " + balance[i]);
            s += (">> " + title[i] + "\t " + balance[i] + " ");
        }

        GM_xmlhttpRequest ( {
            method:     "POST",
            url:        'https://maker.ifttt.com/trigger/bankmoney/with/key/feQcXd0QuePnJb23E97bv',
            data:       JSON.stringify({"value1":s, "value2":"HELLO"}),
            headers:    {
                "Content-Type": "application/json"
            },
            onload:     function (response) {
                console.log ("gut response");
                alert("Success");
            }
        } );

    }
    //create function, it expects 2 values.
    function insertAfter(newElement,targetElement) {
        //target is what you want it to go after. Look for this elements parent.
        var parent = targetElement.parentNode;

        //if the parents lastchild is the targetElement...
        if(parent.lastchild == targetElement) {
            //add the newElement after the target element.
            parent.appendChild(newElement);
        } else {
            // else the target has siblings, insert the new element between the target and it's next sibling.
            parent.insertBefore(newElement, targetElement.nextSibling);
        }
    }



    function getRefObject(){
        var currentLocation = window.location;
        var xpath;

        if(currentLocation.host.indexOf('citibank')>=0){
            xpath='//*[@id="categoryType-BNKCHK"]';
        }
        else if(currentLocation.host.indexOf('hsbc')>=0){
            xpath="//span[@class='row accountAccordionRow']";
        }
		 else if(currentLocation.host.indexOf('hangseng')>=0){
            xpath="//div[@class='messageBar shadow']";
        }
        else{
            alert('Invalid url: ' + currentLocation);
        }

        var tags_data_image =document.evaluate(xpath, document, null, XPathResult.ANY_TYPE,null);
        var t=tags_data_image.iterateNext();
        return t;
    }

	 function main(){

        var btnSave=getRefObject();

        var btn = document.createElement("a");
        btn.innerText='DoMyTask!';
        btn.addEventListener("click",updateAll);


        insertAfter(btn, btnSave);
    }

    debugger;

    window.addEventListener('load', function() {
          var checkExist = setInterval(function() {
            if (getRefObject()!=null) {
                console.log("Exists!");
				main();
                clearInterval(checkExist);
            }
        }, 1000); // check every 100ms
    }, false);

   function clockbtn(){
       var clickEvent      = document.createEvent ('MouseEvents');
       var firstZipFile    = document.querySelector ("a[href*='.zip']");

       clickEvent.initEvent ('click', true, true);
       firstZipFile.dispatchEvent (clickEvent);

       //https://stackoverflow.com/questions/46051619/tampermonkey-gm-download-local-path
   }
    // Your code here...
})();