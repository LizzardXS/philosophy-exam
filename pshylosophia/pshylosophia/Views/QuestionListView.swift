//
//  Untitled.swift
//  pshylosophia
//
//  Created by Елизавета Сенюк on 30.05.25.
//

import SwiftUI

struct QuestionListView: View{
    var body: some View{
        List(qaList){ qa in
            DisclosureGroup(qa.question){
                Text(qa.answer)
                    .padding(.top, 5)
            }
        }
        .toolbar{
            ToolbarItem(placement: .principal) {
                Text("Список вопросов")
                    .padding(.top, 15)
                    .font(.system(size: 20))
                    .fontDesign(.serif)
            }
            
        }
    }
}
