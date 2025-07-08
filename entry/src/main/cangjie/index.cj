/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

package ohos_app_cangjie_entry

import ohos.state_macro_manage.*

@Entry
@Component
class MyView {
    let scroller = Scroller()

    func build() {
        Row {
            Scroll(this.scroller) {
                Column {
                    Button("Compressmp4High").onClick {Router.push(url: "Compressmp4High")}
                    Button("Compressmp4Medium").onClick {Router.push(url: "Compressmp4Medium")}
                    Button("Compressmp4Low").onClick {Router.push(url: "Compressmp4Low")}
                    Button("CompresstxtHigh").onClick {Router.push(url: "CompresstxtHigh")}
                    Button("CompresstxtMedium").onClick {Router.push(url: "CompresstxtMedium")}
                    Button("CompresstxtLow").onClick {Router.push(url: "CompresstxtLow")}
                    Button("Compressing").onClick {Router.push(url: "Compressing")}
                    Button("Compress3type").onClick {Router.push(url: "Compress3type")}
                }.width(100.percent)
            }
        }.height(100.percent)
    }
}
