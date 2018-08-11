// ==UserScript==
// @name         TimeBooking
// @namespace    http://tampermonkey.net/
// @version      0.1
// @description  try to take over the world!
// @author       You
// @match        https://www.citibank.com.hk/*
// @match        https://www.services.online-banking.hsbc.com.hk/*
// @match        https://e-banking1.hangseng.com/*
// @match        https://ebsnew.boc.cn/*
// @match        https://its.bochk.com/cdc.overview.do
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
        var title, balance, types=[],bank='';

        if(currentLocation.host.indexOf('citibank')>=0){
            bank = 'citibank';
            title = getInfo("//a[@id='cmlink_AccountNameLink']");
            balance = getInfo("//div[@class='cT-valueItem']/span[@class='cT-balanceIndicator1 ']");
            for(var i=0; i<balance.length; i++){
                var typeAndBalance = balance[i].trim().split(' ');
                if (typeAndBalance.length!=2){
                    aleret('some problem occurs:' + balance[i]);
                }
                var _type = '';
                if(typeAndBalance[0]=='HKD'){
                    _type = 'HKD';
                    balance[i] = typeAndBalance[1];
                }else if(typeAndBalance[0]=='USD'){
                    _type = 'USD';
                    balance[i] = typeAndBalance[1];
                }
                else aleret('invalid type:' + typeAndBalance[0]);
                types.push(_type);
            }
        }
        else if(currentLocation.host.indexOf('hsbc')>=0){
            bank = 'hsbc';
            title=['khd', 'usd', 'invest', 'hkd292', 'credit'];
            var balances_xpath=["//span[contains(@id,'||HKD||2A833HKD       HKDPVSAV       ||HK')]",
                           "//span[contains(@id,'||USD||2F833USD       USDPVSAV       ||HK')]",
                           "//span[contains(@id,'||HKD||2R833HKD       HKDPVINV       ||HK')]",
                           "//span[contains(@id,'||HKD||2C292HKD       HKDSSSSA       ||HK')]",
                           "//span[contains(@id,'||HKD||3PA  HKD       HKDCRAVR       ||HK')]"];

            balance = [];
            types=['HKD', 'USD','HKD','HKD','HKD'];

            for(var i=0; i<balances_xpath.length; i++){
                var t=getInfo(balances_xpath[i]);
                if(t.length!=1){
                    alert('t.length not good' + t.toString());
                }
                balance.push(t[0]);
            }
        }
		else if(currentLocation.host.indexOf('hangseng')>=0){
            bank='hangseng';


            var Money = getInfo("//table[contains(@id,'SumTable')]/tbody/tr/td[last()]");
            var Currency = getInfo("//table[contains(@id,'SumTable')]/tbody/tr/td[last()-1]");
            var Name = getInfo("//table[contains(@id,'SumTable')]/tbody/tr/td[last()-3]");

            if(Money.length !=Currency.length || Money.length !=Name.length){
                alert('length has problem');
            }

            title=[];
            balance=[];
            types=[];
            for(var i=0; i<Currency.length; i++){
                if(Currency[i].trim().length!=0){
                    title.push(Name[i]);

                    Money[i] = Money[i].trim();
                    var p = Money[i].indexOf('DR');
                    var thisban='';
                    if (p>=0){
                        thisban = '-' + Money[i].substring(0, p);
                    }else thisban = Money[i];
                    balance.push(thisban);

                    types.push(Currency[i]);
                }
            }
        }
        else if(currentLocation.host.indexOf('boc.cn')>=0){
            bank='boc.cn';


            var Money = getInfo("//tr[contains(@class,'balance')]/td[last()-1]");
            var Currency = getInfo("//tr[contains(@class,'balance')]/td[last()-3]");
            var Name = getInfo("//tr[contains(@class,'balance')]/td[last()-3]");

            if(Money.length !=Currency.length || Money.length !=Name.length){
                alert('length has problem');
            }

            title=[];
            balance=[];
            types=[];
            for(var i=0; i<Currency.length; i++){
                if(Currency[i].trim().length!=0){
                    title.push(Name[i]);

                    Money[i] = Money[i].trim();
                    var p = Money[i].indexOf('DR');
                    var thisban='';
                    thisban = Money[i];
                    balance.push(thisban);

                    if(Currency[i].indexOf( '人民币')>=0)
                        types.push('RMB');
                    else if(Currency[i].indexOf('港币')>=0)
                        types.push('HKD');
                    else{
                        alert('unknown currency: ' +Currency[i] );
                        types.push(Currency[i]);
                    }
                }
            }
        }
         else if(currentLocation.host.indexOf('bochk')>=0){
            bank='bochk';


            var Money = getInfo("//table[@class='full_width']//td[@class='data_table_swap1_txt data_table_lastcell'][1]/a");
            var Currency = ['HKD'];
            var Name = getInfo("/html/body/table[@id='mid']/tbody/tr/td[@id='content_cell']/div[@id='content']/table[@class='full_width'][1]/tbody/tr[2]/td[@class='data_table_swap1_txt'][2]/a");

            if(Money.length !=Currency.length || Money.length !=Name.length){
                alert('length has problem');
            }

            title=[];
            balance=[];
            types=[];
            for(var i=0; i<Currency.length; i++){
                if(Currency[i].trim().length!=0){
                    title.push(Name[i]);

                    Money[i] = Money[i].trim();
                    var p = Money[i].indexOf('DR');
                    var thisban='';
                    thisban = Money[i];
                    balance.push(thisban);

                    types.push(Currency[i]);
                }
            }
        }
        else{
            alert('Invalid url: ' + currentLocation);
        }

        if(title.length!=balance.length)
        {
            alert('logic bug!');return;
        }

        var s=[];
        for(i=0; i<title.length;i++){
            s.push({'name':title[i], 'type':types[i], 'balance':balance[i]});
            console.log(">> " + title[i] + "\t " + balance[i]);
        }

        var s2={'bank':bank, 'balance':s};

        var today = new Date();
        var dd = today.getDate();
        var mm = today.getMonth()+1; //January is 0!
        var yyyy = today.getFullYear();

        if(dd<10) {
            dd = '0'+dd
        }

        if(mm<10) {
            mm = '0'+mm
        }

        today = '' + yyyy + mm + dd;
        var sFinal = {"value1":s2, "value2":today};

       // document.write(today);

        alert(JSON.stringify(sFinal));

        GM_xmlhttpRequest ( {
            method:     "POST",
            url:        'https://maker.ifttt.com/trigger/bankmoney/with/key/feQcXd0QuePnJb23E97bv',
            data:       JSON.stringify(sFinal),
            headers:    {
                "Content-Type": "application/json"
            },
            onload:     function (response) {
                console.log ("gut response " + response);
                alert("Success " + response);
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
         else if(currentLocation.host.indexOf('boc.cn')>=0){
            xpath="//p[@class='AccountOverview_breads bread']/span[@class='second']";
        }
        else if(currentLocation.host.indexOf('bochk')>=0){
            xpath="//td[@id='page_title']/h1";
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
        btn.setAttribute('color','red');


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